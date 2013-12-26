#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/mutex.hpp>

#include <sstream>

namespace Log
{
  struct UniqueTime
  {
    UniqueTime()
      : Tune(0)
    {
    }

    UniqueTime(const boost::posix_time::ptime time, const unsigned tune)
      : Time(time)
      , Tune(tune)
    {
    }

    boost::posix_time::ptime Time;
    unsigned Tune;
    bool operator<(const UniqueTime& t2) const;
    std::string ToString() const;
  };
}