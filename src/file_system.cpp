#include "file_system.h"
#include "xml_file_store.h"

#include <boost/filesystem.hpp>

namespace Log
{
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

  std::set<std::string> FileSystem::GetArcList(const std::string& dir) const
  {
    std::set<std::string> files;
    boost::filesystem::path path(dir);
    boost::filesystem::directory_iterator itEnd;

    for (boost::filesystem::directory_iterator itDir(path); itDir != itEnd; ++itDir)
    {
      if (boost::filesystem::is_regular_file(itDir->status()))
      {
        std::string name = itDir->path().string();
        if (name.substr(name.size() - XmlFileStore::ArcSuffix.size()) == XmlFileStore::ArcSuffix)
        {
          files.insert(name);
        }
      }
    }
    return files;
  }
}