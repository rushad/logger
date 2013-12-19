#pragma once

#include "logger.h"

#include <boost/thread/mutex.hpp>

#include <fstream>

namespace Log
{
  class XmlFileInterface
  {
  public:
    virtual ~XmlFileInterface()
    {
    }

    virtual void Write(const std::string& str) = 0;
  };

  class XmlFile : public XmlFileInterface
  {
  public:
    XmlFile(const std::string& dirName, const unsigned maxSize);
    ~XmlFile();
    virtual void Write(const std::string& str);

  private:
    std::string DirName;
    unsigned MaxSize;
    std::ofstream FileStream;

    void Rotate();
  };

  class FakeXmlFile : public XmlFileInterface
  {
  public:
    virtual void Write(const std::string& str);
    std::string GetRecord(unsigned index) const;

  private:
    std::vector<std::string> Records;
  };

  class XmlFileStore : public Store
  {
  public:
    explicit XmlFileStore(XmlFileInterface* file);
    virtual void Add(const EventPtr& theEvent);
 
  private:
    XmlFileInterface* File;
  };
}