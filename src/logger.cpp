#include "logger.h"

#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <pthread.h>

namespace Log
{
  EventQueueThreadLoop::EventQueueThreadLoop(EventQueue& queue, Store& store)
    : Queue(queue)
    , TheStore(store)
  {
  }

  EventQueueThreadLoop::~EventQueueThreadLoop()
  {
    Queue.Close();
  }

  bool EventQueueThreadLoop::LoopBody()
  {
    EventPtr& theEvent = Queue.Get();
    if(!theEvent)
      return false;

    TheStore.Add(theEvent);
    return true;
  }

  Logger::Logger(Store& store, const Verbosity verb)
    : TheStore(store)
    , TheThreadLoop(Queue, TheStore)
    , Verb(verb)
  {
  }

  void Logger::Write(const Verbosity verb, const std::string& category, const std::string& message, const MapTags& tags)
  {
    if(verb < Verb)
      return;

    EventPtr theEvent(new Event);
    theEvent->Verb = verb;
    theEvent->Category = category;
    theEvent->Message = message;
    theEvent->Tags = tags;
    Queue.Put(theEvent);
  }

  void Logger::WaitForFlush()
  {
    Queue.WaitForEmpty();
  }

  Verbosity Logger::GetVerbosity() const
  {
    boost::lock_guard<boost::mutex> locker(LockVerb);
    return Verb;
  }

  void Logger::SetVerbosity(const Verbosity verb)
  {
    boost::lock_guard<boost::mutex> locker(LockVerb);
    Verb = verb;
  }

  const std::string Verb2Str(const Verbosity verb)
  {
    switch(verb)
    {
    case VERB_INFO:
      return "info";
    case VERB_WARNING:
      return "warning";
    case VERB_ERROR:
      return "error";
    default:
      throw std::invalid_argument("Bad Verbosity");
    }
  }
}