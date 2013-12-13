#pragma once

#include "logger.h"

#include <boost/thread/mutex.hpp>

namespace Log
{
  class MemoryStore : public Store
  {
  public:
    virtual void Add(const EventPtr& theEvent);
    EventList Find(const std::string& category) const;
    EventList Find(const boost::posix_time::ptime& t1, const boost::posix_time::ptime& t2) const;

  private:
    mutable boost::mutex LockStore;
    EventList Events;
  };
}