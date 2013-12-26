#pragma once

#include "unique_time.h"

namespace Log
{
  class TimeGenerator
  {
  public:
    TimeGenerator()
      : LastTune(0)
    {
    }

    UniqueTime Now() const;
    UniqueTime GetLastTime() const;

  private:
    mutable boost::posix_time::ptime LastTime;
    mutable unsigned LastTune;
  };
}