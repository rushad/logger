#pragma once

#include "logger.h"
#include "file_system_facade.h"

namespace Log
{
  const size_t DefMaxLogSize = 1024;

  class XmlFileStore : public Store
  {
  public:
    XmlFileStore(const std::string& path, FileSystemFacade& facade, const size_t maxLogSize = DefMaxLogSize);
    virtual void Add(const Event& theEvent);
    std::ostream& GetStream() const;
    void InitStream();
    void Rotate();

  private:
    std::string GetFileName() const;

    const std::string& Path;
    FileSystemFacade& FileSystem;
    std::auto_ptr<std::ostream> Stream;
    const size_t MaxLogSize;
  };
}