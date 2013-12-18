#pragma once

#include "logger.h"

#include <boost/thread/mutex.hpp>

#include <fstream>

namespace Log
{
  class XmlStore : public Store
  {
  public:
    explicit XmlStore(const std::string& dirName);
    ~XmlStore();
    virtual void Add(const EventPtr& theEvent);
 
  private:
    boost::mutex LockFile;
    std::ofstream File;
  };
}