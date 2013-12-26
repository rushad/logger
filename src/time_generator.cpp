#include "time_generator.h"

namespace Log
{
  UniqueTime TimeGenerator::Now() const
  {
    boost::posix_time::ptime time = boost::posix_time::microsec_clock::universal_time();
    unsigned tune;
    if(LastTime == time)
      tune = ++LastTune;
    else
      tune = LastTune = 0;

    LastTime = time;
    return UniqueTime(time, tune);
  }

  UniqueTime TimeGenerator::GetLastTime() const
  {
    return UniqueTime(LastTime, LastTune);
  }
}