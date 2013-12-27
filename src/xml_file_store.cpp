#include "xml_file_store.h"

#include <boost/filesystem.hpp>

#include <set>

namespace Log
{
  const std::string XmlFileStore::ArcSuffix = ".arc.xml";

  XmlFileStore::XmlFileStore(
    const std::string& path, 
    FileSystemFacade& facade, 
    const size_t maxLogSize, 
    const unsigned maxArcCount)
    : Path(path)
    , FileSystem(facade)
    , MaxLogSize(maxLogSize)
    , MaxArcCount(maxArcCount)
  {
    if (!facade.Exists(Path))
      facade.CreateDir(Path);

    const std::string fileName = GetFileName();
    if (!facade.Exists(fileName))
      InitStream();
    else
      Stream = FileSystem.OpenFile(fileName);
  }

  void XmlFileStore::Add(const Event& theEvent)
  {
    std::string str = "  <event";
    str += " time=\"" + theEvent.Time.ToString() + "\"";
    str += " category=\"" + theEvent.Category + "\"";
    str += " verbosity=\"" + Verb2Str(theEvent.Verb) + "\">\n";

    for(MapTags::const_iterator it = theEvent.Tags.begin(), end = theEvent.Tags.end(); it != end; ++it)
      str += "    <" + it->first + ">" + it->second + "</" + it->first + ">\n";

    str += "    " + theEvent.Message + "\n";
    str += "  </event>\n";

    *Stream << str;

    if (size_t(Stream->tellp()) > MaxLogSize)
      Rotate();
  }

  std::ostream& XmlFileStore::GetStream() const
  {
    return *Stream;
  }

  void XmlFileStore::InitStream()
  {
    Stream = FileSystem.CreateFile(GetFileName());
    if (!Stream.get())
      throw std::exception("Can't create file");
    *Stream << "<?xml version=\"1.0\"?>\n<log>\n";
  }

  void XmlFileStore::Rotate()
  {
    *Stream << "</log>\n";
    FileSystem.OnCloseFile(*Stream);
    Stream.reset();

    FileSystem.RenameFile(GetFileName(), Path + "/" + TimeGen.Now().ToString() + ArcSuffix);
    RemoveOldFiles();

    InitStream();
  }

  UniqueTime XmlFileStore::GetLastTime() const
  {
    return TimeGen.GetLastTime();
  }

  std::string XmlFileStore::GetFileName() const
  {
    return Path + "/log.xml";
  }

  void XmlFileStore::RemoveOldFiles() const
  {
    std::auto_ptr<AbstractFileSystemIterator> it = FileSystem.GetIterator(Path, "*" + ArcSuffix);
    std::string name;
    std::set<std::string> fileList;

    while(!(name = it->Next()).empty())
    {
      fileList.insert(name);
    }

    if (fileList.size() > MaxArcCount)
    {
      std::set<std::string>::const_iterator itFile = fileList.begin();
      for (size_t i = 0; i < fileList.size() - MaxArcCount; ++i, ++itFile)
      {
        FileSystem.RemoveFile(*itFile);
      }
    }
  }
}