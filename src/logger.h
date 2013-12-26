#pragma once

#include "atomic_queue.h"
#include "time_generator.h"
#include "thread_loop.h"
#include "unique_time.h"

#include <boost/shared_ptr.hpp>

#include <string>
#include <vector>

namespace Log
{
  const unsigned MaxQueueSize = 1000;

  enum Verbosity
  {
    VERB_INFO,
    VERB_WARNING,
    VERB_ERROR
  };

  typedef std::map<std::string, std::string> MapTags;
  struct Event
  {
    Verbosity Verb;
    UniqueTime Time;
    std::string Category;
    std::string Message;
    MapTags Tags;
  };

  typedef std::vector<Event> EventList;
  typedef AtomicQueue<Event> EventQueue;

  const std::string Verb2Str(const Verbosity verb);

  class Store
  {
  public:
    virtual ~Store()
    {
    }

    virtual void Add(const Event& theEvent) = 0;
  };

  class EventQueueThreadLoop : public ThreadLoop
  {
  public:
    EventQueueThreadLoop(EventQueue& queue, Store& store, boost::condition_variable& queueEmptied);
    ~EventQueueThreadLoop();

  protected:
    virtual bool LoopBody();

  private:
    EventQueue& Queue;
    Store& TheStore;
    boost::condition_variable& QueueEmptied;
  };

  class Logger
  {
  public:
    Logger(Store& store, const Verbosity verb = VERB_WARNING, const unsigned maxQueueSize = MaxQueueSize);
    void Write(const Verbosity verb, const std::string& category, const std::string& message, const MapTags& tags = MapTags());
    void WaitForFlush();
    Verbosity GetVerbosity() const;
    void SetVerbosity(const Verbosity verb);
    UniqueTime Now() const
    {
      return TimeGen.Now();
    }

  public:
    mutable boost::mutex LockVerb;
    boost::mutex LockQueue;
    Verbosity Verb;
    boost::condition_variable QueueEmptied;
    Store& TheStore;
    EventQueue Queue;
    EventQueueThreadLoop TheThreadLoop;
    TimeGenerator TimeGen;
  };
}