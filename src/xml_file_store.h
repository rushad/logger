#pragma once

#include "logger.h"
#include "file_system_facade.h"

namespace Log
{
  const size_t DefMaxLogSize = 1024 * 1024;
  const unsigned DefMaxArcCount = 10;

  class XmlFileStore : public Store
  {
  public:
    XmlFileStore(
      const std::string& path, 
      FileSystemFacade& facade,
      const size_t maxLogSize = DefMaxLogSize, 
      const unsigned maxArcCount = DefMaxArcCount);
    virtual void Add(const Event& theEvent);
    std::ostream& GetStream() const;
    void InitStream();
    void Rotate();
    UniqueTime GetLastTime() const;

  private:
    std::string GetFileName() const;
    void RemoveOldFiles() const;


    const std::string Path;
    FileSystemFacade& FileSystem;
    std::auto_ptr<std::ostream> Stream;
    const size_t MaxLogSize;
    const unsigned MaxArcCount;
    TimeGenerator TimeGen;

  public:
    const static std::string ArcSuffix;
  };
}