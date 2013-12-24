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
        ++RenameFileCallCount;
      }


      std::string LastCreatedDir;
      std::string Name;
      std::string LastStreamContent;
      unsigned CreateDirCallCount;
      unsigned CreateFileCallCount;
      unsigned OpenFileCallCount;
      unsigned RenameFileCallCount;
      std::vector<std::string> ExistedNames;
    };
  }
}