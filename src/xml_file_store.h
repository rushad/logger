#pragma once

#include "logger.h"

#include <boost/thread/mutex.hpp>

#include <fstream>

namespace Log
{
  class XmlFileLogic
  {
  public:
    XmlFileLogic(unsigned maxLogSize);

    virtual ~XmlFileLogic();
//    virtual void OpenFile() = 0;
    virtual unsigned GetFileSize() = 0;

    void Write(const std::string& str);

  protected:
    std::auto_ptr<std::ostream> StreamPtr;
    unsigned Written;

  private:
    virtual void Rotate() = 0;

    unsigned MaxLogSize;
  };

  class XmlFile : public XmlFileLogic
  {
  public:
    XmlFile(const unsigned maxLogSize, const std::string& dirName);
    ~XmlFile();
//    virtual void OpenFile();
    virtual unsigned GetFileSize();

  private:
    virtual void Rotate();

    std::string DirName;
  };
/*
  class FakeXmlFile : public XmlFileInterface
  {
  public:
    FakeXmlFile(const std::string& dirName, const unsigned maxSize);
    virtual void Write(const std::string& str);
    std::string GetRecord(unsigned index) const;
    unsigned GetTotalWritten() const;

  private:
    unsigned MaxSize;
    unsigned TotalWritten;
    std::vector<std::string> Records;
  };
*/
  class XmlFileStore : public Store
  {
  public:
    explicit XmlFileStore(XmlFileLogic* file);
    virtual void Add(const EventPtr& theEvent);
 
  private:
    XmlFileLogic* File;
  };
}