#pragma once

#include "file_system_facade.h"

#include <sstream>

namespace Log
{
  namespace Test
  {
    class FakeFileSystem : public FileSystemFacade
    {
    public:
      FakeFileSystem()
        : CreateDirCallCount(0)
        , CreateFileCallCount(0)
        , OpenFileCallCount(0)
        , RenameFileCallCount(0)
        , RemovedFilesCount(0)
        , ThrowOnCreate(false)
      {
      }

      virtual bool Exists(const std::string& name)
      {
        std::vector<std::string>::const_iterator it = std::find(ExistedNames.begin(), ExistedNames.end(), name);
        return it != ExistedNames.end();
      }

      virtual void CreateDir(const std::string& name)
      {
        ExistedNames.push_back(name);
        LastCreatedDir = name;
        ++CreateDirCallCount;
      }

      virtual std::auto_ptr<std::ostream> CreateFile(const std::string& name)
      {
        if (ThrowOnCreate)
          return std::auto_ptr<std::ostream>();

        ExistedNames.push_back(name);
        Name = name;
        ++CreateFileCallCount;
        return std::auto_ptr<std::ostream>(new std::ostringstream);
      }

      virtual std::auto_ptr<std::ostream> OpenFile(const std::string& name)
      {
        Name = name;
        ++OpenFileCallCount;
        return std::auto_ptr<std::ostream>(new std::ostringstream);
      }

      virtual void OnCloseFile(std::ostream& stream)
      {
        LastStreamContent = static_cast<std::ostringstream&>(stream).str();
      }

      virtual void RenameFile(const std::string& from, const std::string& to)
      {
        RotatedName = to;
        RenamedFiles.insert(to);
        ++RenameFileCallCount;
      }

      virtual void RemoveFile(const std::string& name)
      {
        LastRemovedFile = name;
        ++RemovedFilesCount;
      }

      virtual std::set<std::string> GetArcList(const std::string& dir) const
      {
        return RenamedFiles;
      }

      std::string LastCreatedDir;
      std::string Name;
      std::string LastStreamContent;
      std::string RotatedName;
      std::string LastRemovedFile;
      unsigned CreateDirCallCount;
      unsigned CreateFileCallCount;
      unsigned OpenFileCallCount;
      unsigned RenameFileCallCount;
      unsigned RemovedFilesCount;
      std::vector<std::string> ExistedNames;
      std::set<std::string> RenamedFiles;
      bool ThrowOnCreate;
    };
  }
}