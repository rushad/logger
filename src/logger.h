#pragma once

#include <memory>
#include <string>
#include <vector>

namespace Log
{
  struct Event
  {
    std::string Message;
    std::string Category;
  };

  typedef std::vector<Event> EventList;

  class Store
  {
  public:
    virtual ~Store()
    {
    }

    virtual void Add(const Event& message) = 0;
  };

  class Logger
  {
  public:
    Logger(Store& store);
    void Write(const std::string& category, const std::string& message);
  
  private:
    Store& TheStore;
  };

  class Time
  {
  public:
    Time(const Time& time);
    static Time Now();
  };
}