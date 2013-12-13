#pragma once

#include "atomic_queue.h"
#include "thread_loop.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/shared_ptr.hpp>

#include <string>
#include <vector>

namespace Log
{
  struct Event
  {
    boost::posix_time::ptime Time;
    std::string Category;
    std::string Message;
  };

  typedef boost::shared_ptr<Event> EventPtr;
  typedef std::vector<EventPtr> EventList;

  class Store
  {
  public:
    virtual ~Store()
    {
    }

    virtual void Add(const EventPtr& theEvent) = 0;
  };

  class EventQueueThreadLoop : public ThreadLoop
  {
  public:
    EventQueueThreadLoop(AtomicQueue<EventPtr>* queue, Store& store);

  protected:
    virtual bool LoopBody();

  private:
    AtomicQueue<EventPtr>* Queue;
    Store& TheStore;
  };

  class Logger
  {
  public:
    Logger(Store& store);
    void Write(const std::string& category, const std::string& message);

  private:
    Store& TheStore;
    EventQueueThreadLoop TheThreadLoop;
    AtomicQueue<EventPtr> Queue;
  };
}