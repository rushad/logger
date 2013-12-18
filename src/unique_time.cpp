#include "unique_time.h"

#include <boost/thread/lock_guard.hpp>

namespace Log
{
  boost::mutex UniqueTime::LockStaticData;
  boost::posix_time::ptime UniqueTime::LastTime;
  unsigned UniqueTime::LastTune = 0;

  UniqueTime::UniqueTime()
    : Tune(0)
  {
    Time = GetClockTime();

    boost::lock_guard<boost::mutex> locker(LockStaticData);
    if (Time == LastTime)
      Tune = ++LastTune;
    else
      LastTune = 0;
    LastTime = Time;
  }

  std::string UniqueTime::ToString() const
  {
    std::ostringstream ss;
    boost::posix_time::time_facet *facet = new boost::posix_time::time_facet("%Y%m%d.%H%M%S.%f");
    ss.imbue(std::locale(ss.getloc(), facet));
    ss << Time << "." << Tune;
    return ss.str();
  }

  bool UniqueTime::operator>(const UniqueTime& t2) const
  {
    if(Time == t2.Time)
    {
      return (Tune > t2.Tune);
    }
    return (Time > t2.Time);
  }

  bool UniqueTime::operator<(const UniqueTime& t2) const
  {
    if(Time == t2.Time)
    {
      return (Tune < t2.Tune);
    }
    return (Time < t2.Time);
  }

  bool UniqueTime::operator>=(const UniqueTime& t2) const
  {
    if(Time == t2.Time)
    {
      return (Tune >= t2.Tune);
    }
    return (Time >= t2.Time);
  }

  bool UniqueTime::operator<=(const UniqueTime& t2) const
  {
    if(Time == t2.Time)
    {
      return (Tune <= t2.Tune);
    }
    return (Time <= t2.Time);
  }

   bool UniqueTime::operator==(const UniqueTime& t2) const
  {
    if(Time == t2.Time)
    {
      return (Tune == t2.Tune);
    }
    return false;
  }

  bool UniqueTime::operator!=(const UniqueTime& t2) const
  {
    if(Time == t2.Time)
    {
      return (Tune != t2.Tune);
    }
    return true;
  }

 boost::posix_time::ptime UniqueTime::GetClockTime() const
  {
    return boost::posix_time::microsec_clock::universal_time();
  }
}