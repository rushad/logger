#pragma once

#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>

#include <queue>

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
      Close();
    }

    void Put(const T& data)
    {
      {
        boost::lock_guard<boost::mutex> locker(LockQueue);
        Queue.push_back(data);
      }
      EventArrived.notify_one();
    }

    T Get()
    {
      boost::unique_lock<boost::mutex> locker(LockQueue);
      while (!Stop && Queue.empty())
        EventArrived.wait(locker);

      if (Stop)
        return T();

      T data = Queue.front();
      Queue.pop_front();

      if (Queue.empty())
        QueueEmptied.notify_all();

      return data;
    }

    void Close()
    {
      {
        boost::lock_guard<boost::mutex> locker(LockQueue);
        Stop = true;
      }
      EventArrived.notify_one();
      QueueEmptied.notify_all();
    }

    void WaitForEmpty()
    {
      boost::unique_lock<boost::mutex> locker(LockQueue);
      while (!Stop && !Queue.empty())
        QueueEmptied.wait(locker);
    }


  private:
    mutable boost::mutex LockQueue;
    boost::condition_variable EventArrived;
    boost::condition_variable QueueEmptied;
    std::deque<T> Queue;
    bool Stop;
  };
}