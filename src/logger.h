#pragma once

#include "atomic_queue.h"
#include "thread_loop.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/shared_ptr.hpp>

#include <string>
#include <vector>

namespace Log
{
  enum Verbosity
  {
    VERB_INFO,
    VERB_WARNING,
    VERB_ERROR
  };

  struct Event
  {
    Verbosity Verb;
    boost::posix_time::ptime Time;
    unsigned Tune;
    std::string Category;
    std::string Message;
  };

  typedef boost::shared_ptr<Event> EventPtr;
  typedef std::vector<EventPtr> EventList;
  typedef AtomicQueue<EventPtr> EventQueue;

  const std::string Verb2Str(const Verbosity verb);

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
    EventQueueThreadLoop(EventQueue& queue, Store& store);
    ~EventQueueThreadLoop();

  protected:
    virtual bool LoopBody();

  private:
    EventQueue& Queue;
    Store& TheStore;

    boost::posix_time::ptime LastTime;
    unsigned Tune;
  };

  class Logger
  {
  public:
    Logger(Store& store, Verbosity verb = VERB_WARNING);
    void Write(const Verbosity verb, const std::string& category, const std::string& message);
    void WaitForFlush();
    Verbosity GetVerbosity() const;
    void SetVerbosity(const Verbosity verb);

  private:
    mutable boost::mutex LockVerb;
    Verbosity Verb;
    Store& TheStore;
    EventQueue Queue;
    EventQueueThreadLoop TheThreadLoop;
  };

  const std::string Verb2Str(const Verbosity verb);
}