#pragma once

#include "file_system_facade.h"

#include <fstream>

namespace Log
{
  class FileSystem : public FileSystemFacade
  {
  public:
    virtual bool Exists(const std::string& name);
    virtual void CreateDir(const std::string& name);
    virtual std::auto_ptr<std::ostream> CreateFile(const std::string& name);
    virtual std::auto_ptr<std::ostream> OpenFile(const std::string& name);
    virtual void OnCloseFile(std::ostream& stream);
  };
}