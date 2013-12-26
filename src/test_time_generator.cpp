#include "time_generator.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include <gtest/gtest.h>

namespace Log
{
  namespace Test
  {
    class TestTimeGenerator : public ::testing::Test
    {
    };

    TEST_F(TestTimeGenerator, ShouldGenerateNewTime)
    {
      TimeGenerator tg;
      UniqueTime ut = tg.Now();
      ASSERT_NE(boost::posix_time::not_a_date_time, ut.Time);
    }

    TEST_F(TestTimeGenerator, NextTimeShouldBeDifferent)
    {
      TimeGenerator tg;
      UniqueTime ut1 = tg.Now();
      UniqueTime ut2 = tg.Now();
      ASSERT_TRUE(ut1 < ut2);
    }
  }
}