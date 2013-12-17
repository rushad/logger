#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/mutex.hpp>

#include <sstream>

namespace Log
{
  class UniqueTime
  {
  public:
    UniqueTime();
    std::string ToString() const;
    bool operator>(const UniqueTime& t2) const;
    bool operator<(const UniqueTime& t2) const;
    bool operator>=(const UniqueTime& t2) const;
    bool operator<=(const UniqueTime& t2) const;
    bool operator==(const UniqueTime& t2) const;
    bool operator!=(const UniqueTime& t2) const;

  protected:
    virtual boost::posix_time::ptime GetClockTime() const;

  private:
    boost::posix_time::ptime Time;
    unsigned Tune;

    static boost::mutex LockStaticData;
    static boost::posix_time::ptime LastTime;
    static unsigned LastTune;
  };
}