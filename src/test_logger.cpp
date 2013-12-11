#include "logger.h"

#include <gtest/gtest.h>

namespace Log
{
  namespace Test
  {
    class TestLogger : public ::testing::Test
    {
    };

    TEST_F(TestLogger, TestBackendStream)
    {
      std::ostringstream oss;
      LoggerPtr log = CreateLogger(BackendPtr(new Log::BackendStream(oss)));
      log->write("Hello world!");
      EXPECT_EQ("Hello world!", oss.str());
    }

    //TEST_F(TestLogger, LoggerShouldStoreMessage)
    //{
    //  Logger log;
    //  log.write("category", "message");

    //  std::vector<std::string> messages = log.find("category");

    //  ASSERT_EQ(1, messages.size());
    //  ASSERT_EQ("message", messages[0]);
    //}
  }
}