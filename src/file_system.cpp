#include "file_system.h"

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
    rename(from.c_str(), to.c_str());
  }
}