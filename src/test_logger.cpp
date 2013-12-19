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
    public:
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

    TEST_F(TestLogger, NoDelaysWhenQueueSpaceSufficient)
    {
      const unsigned delay = 100;
      SlowMemoryStore store(delay);
      Logger log(store, VERB_INFO, 0);

      boost::posix_time::ptime t1(boost::posix_time::microsec_clock::universal_time());
      log.Write(VERB_INFO, "category", "message");
      boost::posix_time::ptime t2(boost::posix_time::microsec_clock::universal_time());

      boost::posix_time::ptime te(t1 + boost::posix_time::millisec(delay));
      ASSERT_GT(te, t2);
    }

    TEST_F(TestLogger, DelayWhenQueueSpaceInsufficient)
    {
      unsigned maxQueueSize = 50;
      const unsigned numIter(100);
      ASSERT_GE(numIter, maxQueueSize);

      const unsigned delay = 1;
      SlowMemoryStore store(delay);
      Logger log(store, VERB_INFO, maxQueueSize);

      boost::posix_time::ptime t1(boost::posix_time::microsec_clock::universal_time());

      for(int i = 0; i < numIter; ++i)
        log.Write(VERB_INFO, "category", "message");

      boost::posix_time::ptime t2(boost::posix_time::microsec_clock::universal_time());

      boost::posix_time::ptime t2e(t1 + boost::posix_time::millisec((numIter - maxQueueSize) * delay * 2));
      EXPECT_GE(t2, t2e);
    }

    TEST_F(TestLogger, WaitForFlush)
    {
      unsigned maxQueueSize = 50;
      const unsigned numIter(100);
      ASSERT_GE(numIter, maxQueueSize);

      const unsigned delay = 1;
      SlowMemoryStore store(delay);
      Logger log(store, VERB_INFO, maxQueueSize);

      for(int i = 0; i < numIter; ++i)
        log.Write(VERB_INFO, "category", "message");

      log.WaitForFlush();

      EventList events = store.Find("category");
      EXPECT_EQ(numIter, events.size());
    }

    TEST_F(TestLogger, LogShouldConsiderVerbosity)
    {
      Log.SetVerbosity(VERB_INFO);
      std::string category = "SetVerbosity(VERB_INFO)";
      Log.Write(VERB_INFO, category, "message1");
      Log.Write(VERB_WARNING, category, "message2");
      Log.Write(VERB_ERROR, category, "message3");

      Log.SetVerbosity(VERB_WARNING);
      category = "SetVerbosity(VERB_WARNING)";
      Log.Write(VERB_INFO, category, "message1");
      Log.Write(VERB_WARNING, category, "message2");
      Log.Write(VERB_ERROR, category, "message3");

      Log.SetVerbosity(VERB_ERROR);
      category = "SetVerbosity(VERB_ERROR)";
      Log.Write(VERB_INFO, category, "message1");
      Log.Write(VERB_WARNING, category, "message2");
      Log.Write(VERB_ERROR, category, "message3");

      Log.WaitForFlush();

      EventList events = Store.Find("SetVerbosity(VERB_INFO)");
      EXPECT_EQ(3, events.size());

      events = Store.Find("SetVerbosity(VERB_WARNING)");
      EXPECT_EQ(2, events.size());

      events = Store.Find("SetVerbosity(VERB_ERROR)");
      EXPECT_EQ(1, events.size());
    }
  }
}