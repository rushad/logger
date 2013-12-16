#include "logger.h"
#include "memory_store.h"

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

    TEST_F(TestLogger, LogShouldStoreTime)
    {
      boost::posix_time::ptime t1(boost::posix_time::microsec_clock::universal_time());
      Log.Write(VERB_INFO, "category1", "message1");
      boost::posix_time::ptime t2(boost::posix_time::microsec_clock::universal_time());

      boost::this_thread::sleep_for(boost::chrono::milliseconds(1));

      Log.Write(VERB_INFO, "category2", "message2");

      Log.WaitForFlush();

      EventList events = Store.Find(t1, t2);

      ASSERT_EQ(1, events.size());
      ASSERT_EQ("message1", events[0]->Message);
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

    TEST_F(TestLogger, LogShouldTuneTime)
    {
      std::string category("category");
      Log.Write(VERB_INFO, category, "message1");
      Log.Write(VERB_INFO, category, "message2");
      Log.Write(VERB_INFO, category, "message3");
      Log.Write(VERB_INFO, category, "message4");
      Log.Write(VERB_INFO, category, "message5");

      Log.WaitForFlush();

      EventList events = Store.Find(category);

      bool f = false;
      for (size_t i = 0; i < events.size(); ++i)
      {
        if (events[i]->Tune != 0)
        {
          f = true;
        }
      }
      ASSERT_EQ(true, f);
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