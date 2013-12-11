#pragma once

#include <iostream>
#include <memory>
#include <ostream>
#include <string>

namespace Log
{
  class Backend
  {
  public:
    virtual ~Backend()
    {
    }

    virtual void write(const std::string& str) const = 0;
  };
  typedef std::auto_ptr<Backend> BackendPtr;

  class Logger
  {
    BackendPtr Backend;
  public:
    explicit Logger(BackendPtr backend)
      : Backend(backend)
    {
    }
    void write(const std::string& str) const;
  };
  typedef std::auto_ptr<Logger> LoggerPtr;

  class BackendStream : public Backend
  {
    std::ostream& Stream;
  public:
    explicit BackendStream(std::ostream& stream = std::cout)
      : Stream(stream)
    {
    }

    virtual void write(const std::string& str) const
    {
      Stream << str;
    }
  };

  LoggerPtr CreateLogger(BackendPtr backend = BackendPtr(new BackendStream));
}