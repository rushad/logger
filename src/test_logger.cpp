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
        : Log(Store)
      {
      }

    protected:
      MemoryStore Store;
      Logger Log;
    };

    TEST_F(TestLogger, LoggerShouldStoreMessage)
    {
      Log.Write("category1", "message1");
      Log.Write("category2", "message2");

      boost::this_thread::sleep_for(boost::chrono::milliseconds(100));

      EventList messages = Store.Find("category1");

      ASSERT_EQ(1, messages.size());
      ASSERT_EQ("message1", messages[0]->Message);
    }

    TEST_F(TestLogger, LogShouldStoreCategory)
    {
      Log.Write("category1", "message1");
      Log.Write("category2", "message2");

      boost::this_thread::sleep_for(boost::chrono::milliseconds(100));

      EventList messages = Store.Find("category1");

      ASSERT_EQ(1, messages.size());
      ASSERT_EQ("category1", messages[0]->Category);
    }

    TEST_F(TestLogger, LogShouldStoreTime)
    {
      boost::posix_time::ptime t1(boost::posix_time::microsec_clock::universal_time());
      Log.Write("category1", "message1");
      boost::posix_time::ptime t2(boost::posix_time::microsec_clock::universal_time());

      boost::this_thread::sleep_for(boost::chrono::milliseconds(1));

      Log.Write("category2", "message2");

      boost::this_thread::sleep_for(boost::chrono::milliseconds(100));

      EventList messages = Store.Find(t1, t2);

      ASSERT_EQ(1, messages.size());
      ASSERT_EQ("message1", messages[0]->Message);
    }

    TEST_F(TestLogger, LogShouldNotDelayCallingThread)
    {
      const unsigned delay = 100;
      SlowMemoryStore store(delay);
      Logger log(store);

      boost::posix_time::ptime t1(boost::posix_time::microsec_clock::universal_time());
      log.Write("category", "message");
      boost::posix_time::ptime t2(boost::posix_time::microsec_clock::universal_time());

      boost::posix_time::ptime te(t1 + boost::posix_time::millisec(delay));
      ASSERT_GE(te, t2);
    }
  }
}