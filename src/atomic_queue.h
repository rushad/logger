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
    explicit AtomicQueue(unsigned maxSize)
      : MaxSize(maxSize)
      , Stop(false)
    {
    }

    ~AtomicQueue()
    {
      Close();
    }

    void Put(const T& data)
    {
      {
        boost::unique_lock<boost::mutex> locker(LockQueue);
        if(MaxSize)
        {
          while (!Stop && (Queue.size() >= MaxSize))
            EventProcessed.wait(locker);
        }

        Queue.push_back(data);
      }

      EventArrived.notify_one();
    }

    T Get(bool& stop)
    {
      boost::unique_lock<boost::mutex> locker(LockQueue);
      while (!Stop && Queue.empty())
        EventArrived.wait(locker);

      stop = false;
      if (Stop)
      {
        stop = true;
        return T();
      }

      T data(Queue.front());
      Queue.pop_front();

      EventProcessed.notify_one();
      return data;
    }

    void Close()
    {
      {
        boost::lock_guard<boost::mutex> locker(LockQueue);
        Stop = true;
      }
      EventArrived.notify_one();
      EventProcessed.notify_one();
    }

    bool IsEmpty() const
    {
      boost::lock_guard<boost::mutex> locker(LockQueue);
      return Queue.empty();
    }

  private:
    mutable boost::mutex LockQueue;
    boost::condition_variable EventArrived;
    boost::condition_variable EventProcessed;
    std::deque<T> Queue;
    unsigned MaxSize;
    bool Stop;
  };
}