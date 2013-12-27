#include "file_system.h"
#include "xml_file_store.h"

namespace Log
{
  FileSystemIterator::FileSystemIterator(const std::string& pathName, const std::string& mask)
    : DirIterator(pathName)
    , Mask(mask)
  {
  }

  std::string FileSystemIterator::Next()
  {
    boost::filesystem::directory_iterator itEnd;
    while (DirIterator != itEnd)
    {
      std::string name = DirIterator->path().string();
      bool isFile = boost::filesystem::is_regular_file(DirIterator->status());
      ++DirIterator;
      if (isFile && (name.substr(name.size() - XmlFileStore::ArcSuffix.size()) == XmlFileStore::ArcSuffix))
      {
        return name;
      }
    }
    return "";
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

  std::auto_ptr<AbstractFileSystemIterator> FileSystem::GetIterator(const std::string& pathName, const std::string& mask)
  {
    return std::auto_ptr<AbstractFileSystemIterator>(new FileSystemIterator(pathName, mask));
  }
}