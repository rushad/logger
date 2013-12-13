#include "thread_loop.h"

#include <boost/thread/thread.hpp>

#include <exception>

namespace Log
{
  ThreadLoop::ThreadLoop()
    : Stop(false)
  {
    if(pthread_create(&ThreadId, 0, Run, this))
      throw std::exception("pthread_create() failed");
  }

  ThreadLoop::~ThreadLoop()
  {
    Stop = true;
    pthread_join(ThreadId, 0);
  }

  void* ThreadLoop::Run(void* data)
  {
    ThreadLoop* loop = (ThreadLoop*)data;

    while(!loop->Stop)
    {
      if(!loop->LoopBody())
      {
        break;
      }
    }

    return 0;
  }
}