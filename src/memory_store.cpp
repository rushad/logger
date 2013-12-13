#include "memory_store.h"

namespace Log
{
  void MemoryStore::Add(const EventPtr& theEvent)
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
      EventPtr theEvent = *it;
      if (theEvent->Category == category)
      {
        result.push_back(theEvent);
      }
    }

    return result;
  }

  EventList MemoryStore::Find(const boost::posix_time::ptime& t1, const boost::posix_time::ptime& t2) const
  {
    boost::lock_guard<boost::mutex> locker(LockStore);
    EventList result;

    for (EventList::const_iterator it = Events.begin(), end = Events.end() ; it != end ; ++it)
    {
      EventPtr theEvent = *it;
      if ((theEvent->Time >= t1) && (theEvent->Time <= t2))
      {
        result.push_back(theEvent);
      }
    }

    return result;
  }

}