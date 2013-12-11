#include "logger.h"
#include "memory_store.h"

#include <gtest/gtest.h>

namespace Log
{
  namespace Test
  {
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
      Log.Write("category", "message1");
      Log.Write("category2", "message2");

      EventList messages = Store.Find("category");

      ASSERT_EQ(1, messages.size());
      ASSERT_EQ("message1", messages[0].Message);
    }

    TEST_F(TestLogger, LogShouldStoreCategory)
    {
      Log.Write("category", "message1");
      Log.Write("category2", "message2");

      EventList messages = Store.Find("category");

      ASSERT_EQ(1, messages.size());
      ASSERT_EQ("category", messages[0].Category);
    }

    TEST_F(TestLogger, LogShouldStoreTime)
    {
/*      Time t1(Time::Now());
      Log.Write("category", "message1");
      Time t2(Time::Now());
      Log.Write("category2", "message2");

      EventList messages = Store.Find(t1, t2);

      ASSERT_EQ(1, messages.size());
      ASSERT_EQ("message1", messages[0].Message);*/
    }
  }
}