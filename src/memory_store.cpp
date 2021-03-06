#include "memory_store.h"

namespace Log
{
  void MemoryStore::Add(const Event& theEvent)
  {
    boost::lock_guard<boost::mutex> locker(LockStore);
    Events.push_back(theEvent);
  }

  EventList MemoryStore::Find(const std::string& category) const
  {
    boost::lock_guard<boost::mutex> locker(LockStore);
    EventList result;

    for (EventList::const_iterator it = Events.begin(), end = Events.end() ; it != end ; ++it)
    {
      Event theEvent = *it;
      if (theEvent.Category == category)
      {
        result.push_back(theEvent);
      }
    }

    return result;
  }

  EventList MemoryStore::Find(const UniqueTime& t1, const UniqueTime& t2) const
  {
    boost::lock_guard<boost::mutex> locker(LockStore);
    EventList result;

    for (EventList::const_iterator it = Events.begin(), end = Events.end() ; it != end ; ++it)
    {
      Event theEvent = *it;
      if ((t1 < theEvent.Time) && (theEvent.Time < t2))
      {
        result.push_back(theEvent);
      }
    }

    return result;
  }

}