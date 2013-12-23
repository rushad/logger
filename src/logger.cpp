#include "logger.h"

#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <pthread.h>

namespace Log
{
  EventQueueThreadLoop::EventQueueThreadLoop(EventQueue& queue, Store& store, boost::condition_variable& queueEmptied)
    : Queue(queue)
    , TheStore(store)
    , QueueEmptied(queueEmptied)
  {
  }

  EventQueueThreadLoop::~EventQueueThreadLoop()
  {
    Queue.Close();
  }

  bool EventQueueThreadLoop::LoopBody()
  {
    bool stop = false;
    Event& theEvent = Queue.Get(stop);
    if(stop)
      return false;

    TheStore.Add(theEvent);

    if(Queue.IsEmpty())
      QueueEmptied.notify_one();

    return true;
  }

  Logger::Logger(Store& store, const Verbosity verb, const unsigned maxQueueSize)
    : Verb(verb)
    , TheStore(store)
    , Queue(maxQueueSize)
    , TheThreadLoop(Queue, TheStore, QueueEmptied)
  {
  }

  void Logger::Write(const Verbosity verb, const std::string& category, const std::string& message, const MapTags& tags)
  {
    if(verb < Verb)
      return;

    Event theEvent;
    theEvent.Verb = verb;
    theEvent.Category = category;
    theEvent.Message = message;
    theEvent.Tags = tags;
    Queue.Put(theEvent);
  }

  void Logger::WaitForFlush()
  {
    boost::unique_lock<boost::mutex> locker(LockQueue);
    while (!Queue.IsEmpty())
      QueueEmptied.wait(locker);
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