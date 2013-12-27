#pragma once

#include "file_system_facade.h"

#include <boost/filesystem.hpp>

#include <fstream>

namespace Log
{
  class FileSystemIterator : public AbstractFileSystemIterator
  {
  public:
    FileSystemIterator(const std::string& pathName, const std::string& mask);
    virtual std::string Next();
  private:
    boost::filesystem::directory_iterator DirIterator;
    std::string Mask;
  };

  class FileSystem : public FileSystemFacade
  {
  public:
    virtual bool Exists(const std::string& name);
    virtual void CreateDir(const std::string& name);
    virtual std::auto_ptr<std::ostream> CreateFile(const std::string& name);
    virtual std::auto_ptr<std::ostream> OpenFile(const std::string& name);
    virtual void OnCloseFile(std::ostream& stream);
    virtual void RenameFile(const std::string& from, const std::string& to);
    virtual void RemoveFile(const std::string& name);
    virtual std::auto_ptr<AbstractFileSystemIterator> GetIterator(const std::string& pathName, const std::string& mask);
 };
}