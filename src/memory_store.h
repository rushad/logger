#pragma once

#include "logger.h"

namespace Log
{
  class MemoryStore : public Store
  {
  public:
    virtual void Add(const Event& theEvent);
    EventList Find(const std::string& category) const;

  private:
    EventList Events;
  };
}