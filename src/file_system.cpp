#include "file_system.h"
#include "xml_file_store.h"

namespace Log
{
  FileSystemIterator::FileSystemIterator(const std::string& pathName)
    : DirIterator(pathName)
  {
  }

  bool FileSystemIterator::IsFile() const
  {
    return boost::filesystem::is_regular_file(DirIterator->status());
  }

  std::string FileSystemIterator::FileName() const
  {
    return DirIterator->path().string();
  }

  bool FileSystemIterator::Next()
  {
    boost::filesystem::directory_iterator itEnd;
    return (++DirIterator != itEnd);
  }

  bool FileSystem::Exists(const std::string& name)
  {
    return boost::filesystem::exists(name);
  }

  void FileSystem::CreateDir(const std::string& name)
  {
    boost::filesystem::create_directories(name);
  }

  std::auto_ptr<std::ostream> FileSystem::CreateFile(const std::string& name)
  {
    return std::auto_ptr<std::ostream>(new std::ofstream(name.c_str(), std::ofstream::trunc));
  }

  std::auto_ptr<std::ostream> FileSystem::OpenFile(const std::string& name)
  {
    return std::auto_ptr<std::ostream>(new std::ofstream(name.c_str(), std::ofstream::ate | std::ofstream::app));
  }

  void FileSystem::OnCloseFile(std::ostream& stream)
  {
  }

  void FileSystem::RenameFile(const std::string& from, const std::string& to)
  {
    boost::filesystem::rename(from, to);
  }

  void FileSystem::RemoveFile(const std::string& name)
  {
    boost::filesystem::remove(name);
  }

  std::auto_ptr<AbstractFileSystemIterator> FileSystem::GetIterator(const std::string& pathName)
  {
    return std::auto_ptr<AbstractFileSystemIterator>(new FileSystemIterator(pathName));
  }
}