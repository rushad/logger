#pragma once

#include "boost/thread/condition_variable.hpp"
#include "boost/thread/mutex.hpp"

#include <queue>

#include "logger.h"

namespace Log
{
  template <class T> 
  class AtomicQueue
  {
  public:
    AtomicQueue()
      : Stop(false)
    {
    }

    ~AtomicQueue()
    {
      {
        boost::lock_guard<boost::mutex> locker(LockQueue);
        Stop = true;
      }
      CheckQueue.notify_one();
    }

    void Put(const T& data)
    {
      {
        boost::lock_guard<boost::mutex> locker(LockQueue);
        Queue.push_back(data);
      }
      CheckQueue.notify_one();
    }

    T Get()
    {
      boost::unique_lock<boost::mutex> locker(LockQueue);
      while (!Stop && Queue.empty())
        CheckQueue.wait(locker);

      if (Stop)
        return T();

      T data = Queue.front();
      Queue.pop_front();

      return data;
  }

  private:
    boost::mutex LockQueue;
    boost::condition_variable CheckQueue;
    std::deque<T> Queue;
    bool Stop;
  };
}