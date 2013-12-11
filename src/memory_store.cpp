#include "memory_store.h"

namespace Log
{
  void MemoryStore::Add(const Event& theEvent)
  {
    Events.push_back(theEvent);
  }

  EventList MemoryStore::Find(const std::string& category) const
  {
    EventList result;

    for (EventList::const_iterator it = Events.begin(), end = Events.end() ; it != end ; ++it)
    {
      if (it->Category == category)
      {
        result.push_back(*it);
      }
    }

    return result;
  }
}