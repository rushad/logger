#include "logger.h"

#include <boost/thread/thread.hpp>
#include <pthread.h>

namespace Log
{
  EventQueueThreadLoop::EventQueueThreadLoop(AtomicQueue<EventPtr>* queue, Store& store)
    : Queue(queue)
    , TheStore(store)
  {
  }

  bool EventQueueThreadLoop::LoopBody()
  {
    EventPtr& theEvent = Queue->Get();
    TheStore.Add(theEvent);
    return true;
  }

  Logger::Logger(Store& store)
    : TheStore(store)
    , TheThreadLoop(&Queue, TheStore)
  {
  }

  void Logger::Write(const std::string& category, const std::string& message)
  {
    EventPtr theEvent(new Event);
    theEvent->Time = boost::posix_time::microsec_clock::universal_time();
    theEvent->Category = category;
    theEvent->Message = message;
    Queue.Put(theEvent);
  }
}