#pragma once

#include "logger.h"
#include "unique_time.h"

#include <boost/thread/mutex.hpp>

namespace Log
{
  class MemoryStore : public Store
  {
  public:
    virtual void Add(const Event& theEvent);
    EventList Find(const std::string& category) const;
    EventList Find(const UniqueTime& t1, const UniqueTime& t2) const;

  private:
    mutable boost::mutex LockStore;
    EventList Events;
  };
}