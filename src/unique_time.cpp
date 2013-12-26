#include "unique_time.h"

#include <boost/thread/lock_guard.hpp>

namespace Log
{
  std::string UniqueTime::ToString() const
  {
    std::ostringstream ss;
    boost::posix_time::time_facet *facet = new boost::posix_time::time_facet("%Y%m%d.%H%M%S.%f");
    ss.imbue(std::locale(ss.getloc(), facet));
    ss << Time << "." << Tune;
    return ss.str();
  }

  bool UniqueTime::operator<(const UniqueTime& t2) const
  {
    if(Time == t2.Time)
    {
      return (Tune < t2.Tune);
    }
    return (Time < t2.Time);
  }
}