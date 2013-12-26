#pragma once

#include <fstream>
#include <memory>
#include <set>

namespace Log
{
  class FileSystemFacade
  {
  public:
    ~FileSystemFacade()
    {
    }

    virtual bool Exists(const std::string& name) = 0;
    virtual void CreateDir(const std::string& name) = 0;
    virtual std::auto_ptr<std::ostream> CreateFile(const std::string& name) = 0;
    virtual std::auto_ptr<std::ostream> OpenFile(const std::string& name) = 0;
    virtual void OnCloseFile(std::ostream& stream) = 0;
    virtual void RenameFile(const std::string& from, const std::string& to) = 0;
    virtual void RemoveFile(const std::string& name) = 0;
    virtual std::set<std::string> GetArcList(const std::string& dir) const = 0;
  };
}