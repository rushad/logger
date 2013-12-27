#pragma once

#include "file_system_facade.h"

#include <sstream>

namespace Log
{
  namespace Test
  {
    class FakeFileSystemIterator : public AbstractFileSystemIterator
    {
    public:
      FakeFileSystemIterator(std::vector<std::string> fileList)
        : FileList(fileList)
        , CurIndex(0)
      {
      }

      virtual std::string Next()
      {
        if(CurIndex >= FileList.size())
          return "";
        return FileList[CurIndex++];
      }

    private:
      std::vector<std::string> FileList;
      unsigned CurIndex;
    };

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
        RenamedFiles.push_back(to);
        ++RenameFileCallCount;
      }

      virtual void RemoveFile(const std::string& name)
      {
        LastRemovedFile = name;
        ++RemovedFilesCount;

        std::vector<std::string>::iterator it = std::find(ExistedNames.begin(), ExistedNames.end(), name);
        if (it != ExistedNames.end())
          ExistedNames.erase(it);

        it = std::find(RenamedFiles.begin(), RenamedFiles.end(), name);
        if (it != RenamedFiles.end())
          RenamedFiles.erase(it);
      }

      virtual std::auto_ptr<AbstractFileSystemIterator> GetIterator(const std::string&, const std::string&)
      {
        return std::auto_ptr<AbstractFileSystemIterator>(new FakeFileSystemIterator(RenamedFiles));
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
      std::vector<std::string> RenamedFiles;
      bool ThrowOnCreate;
    };
  }
}