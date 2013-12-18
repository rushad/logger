#include "logger.h"
#include "memory_store.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include <gtest/gtest.h>

#include <windows.h>

namespace Log
{
  namespace Test
  {
    class SlowMemoryStore : public MemoryStore
    {
    public:
      SlowMemoryStore(unsigned delay)
        : Delay(delay)
      {
      }

      virtual void Add(const EventPtr& theEvent)
      {
        boost::this_thread::sleep_for(boost::chrono::milliseconds(Delay));
        MemoryStore::Add(theEvent);
        boost::this_thread::sleep_for(boost::chrono::milliseconds(Delay));
      }

    private:
      unsigned Delay;
    };

    class DumbUniqueTime : public UniqueTime
    {
    protected:
      virtual boost::posix_time::ptime GetClockTime() const
      {
        return DumbTime;
      }

    private:
      const static boost::posix_time::ptime DumbTime;
    };

    const boost::posix_time::ptime DumbUniqueTime::DumbTime = boost::posix_time::microsec_clock::universal_time();

    class TestLogger : public ::testing::Test
    {
    protected:
      TestLogger()
        : Log(Store, VERB_INFO)
      {
      }

    protected:
      MemoryStore Store;
      Logger Log;
    };

    TEST_F(TestLogger, LoggerShouldStoreMessage)
    {
      Log.Write(VERB_INFO, "category1", "message1");
      Log.Write(VERB_INFO, "category2", "message2");

      Log.WaitForFlush();

      EventList events = Store.Find("category1");

      ASSERT_EQ(1, events.size());
      ASSERT_EQ("message1", events[0]->Message);
    }

    TEST_F(TestLogger, LogShouldStoreCategory)
    {
      Log.Write(VERB_INFO, "category1", "message1");
      Log.Write(VERB_INFO, "category2", "message2");

      Log.WaitForFlush();

      EventList events = Store.Find("category1");

      ASSERT_EQ(1, events.size());
      ASSERT_EQ("category1", events[0]->Category);
    }

    TEST_F(TestLogger, LogShouldStoreTags)
    {
      MapTags tags;
      tags["first"] = "First tag";
      tags["second"] = "Second tag";

      Log.Write(VERB_INFO, "category", "message", tags);
      Log.WaitForFlush();

      EventList events = Store.Find("category");
      ASSERT_EQ(2, events[0]->Tags.size());
      ASSERT_EQ("First tag", events[0]->Tags["first"]);
      ASSERT_EQ("Second tag", events[0]->Tags["second"]);
    }

    TEST_F(TestLogger, LogShouldStoreTime)
    {
      UniqueTime t1;
      Log.Write(VERB_INFO, "category1", "message1");
      UniqueTime t2;

      boost::this_thread::sleep_for(boost::chrono::milliseconds(1));

      Log.Write(VERB_INFO, "category2", "message2");

      Log.WaitForFlush();

      EventList events = Store.Find(t1, t2);

      ASSERT_EQ(1, events.size());
      ASSERT_EQ("message1", events[0]->Message);
    }

    TEST_F(TestLogger, TimeShouldBeUnique)
    {
      DumbUniqueTime t1;
      DumbUniqueTime t2;
      ASSERT_EQ(false, (t1 == t2));
    }

    TEST_F(TestLogger, LogShouldNotDelayCallingThread)
    {
      const unsigned delay = 100;
      SlowMemoryStore store(delay);
      Logger log(store, VERB_INFO);

      boost::posix_time::ptime t1(boost::posix_time::microsec_clock::universal_time());
      log.Write(VERB_INFO, "category", "message");
      boost::posix_time::ptime t2(boost::posix_time::microsec_clock::universal_time());

      boost::posix_time::ptime te(t1 + boost::posix_time::millisec(delay));
      ASSERT_GE(te, t2);
    }

    void LogShouldDelayCallingThreadWhenQueueIsOutOfSpace(const unsigned maxQueueSize)
    {
      const unsigned numIter(100);
      const unsigned delay = 1;
      SlowMemoryStore store(delay);
      Logger log(store, VERB_INFO, maxQueueSize);

      boost::posix_time::ptime t1(boost::posix_time::microsec_clock::universal_time());
      for(int i = 0; i < numIter; ++i)
        log.Write(VERB_INFO, "category", "message");

      boost::posix_time::ptime t2(boost::posix_time::microsec_clock::universal_time());
      log.WaitForFlush();
      boost::posix_time::ptime t3(boost::posix_time::microsec_clock::universal_time());

      boost::posix_time::ptime t2e(t1 + boost::posix_time::millisec(maxQueueSize?max(0, (int)numIter - (int)maxQueueSize) * delay * 2 : 0));
      boost::posix_time::ptime t3e(t1 + boost::posix_time::millisec(numIter * delay * 2));

      EXPECT_GE(t2, t2e);
      EXPECT_GE(t3, t3e);

      EventList events = store.Find("category");
      EXPECT_EQ(numIter, events.size());
    }

    TEST_F(TestLogger, LogShouldDelayCallingThreadWhenQueueIsOutOfSpace)
    {
      LogShouldDelayCallingThreadWhenQueueIsOutOfSpace(0);
      LogShouldDelayCallingThreadWhenQueueIsOutOfSpace(1);
      LogShouldDelayCallingThreadWhenQueueIsOutOfSpace(50);
    }

    TEST_F(TestLogger, LogShouldConsiderVerbosity)
    {
      Log.SetVerbosity(VERB_INFO);
      Log.Write(VERB_INFO, "info", "message1");
      Log.Write(VERB_WARNING, "info", "message2");
      Log.Write(VERB_ERROR, "info", "message3");

      Log.SetVerbosity(VERB_WARNING);
      Log.Write(VERB_INFO, "warning", "message1");
      Log.Write(VERB_WARNING, "warning", "message2");
      Log.Write(VERB_ERROR, "warning", "message3");

      Log.SetVerbosity(VERB_ERROR);
      Log.Write(VERB_INFO, "error", "message1");
      Log.Write(VERB_WARNING, "error", "message2");
      Log.Write(VERB_ERROR, "error", "message3");

      Log.WaitForFlush();

      EventList events = Store.Find("info");
      EXPECT_EQ(3, events.size());

      events = Store.Find("warning");
      EXPECT_EQ(2, events.size());

      events = Store.Find("error");
      EXPECT_EQ(1, events.size());
    }
  }
}