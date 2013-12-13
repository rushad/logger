#pragma once

#include <pthread.h>

namespace Log
{
  class ThreadLoop
  {
  public:
    ThreadLoop();
    virtual ~ThreadLoop();

  protected:
    virtual bool LoopBody() = 0;

  private:
    volatile bool Stop;
    pthread_t ThreadId;

    static void* Run(void* data);
  };
}