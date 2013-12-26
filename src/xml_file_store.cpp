#include "xml_file_store.h"

#include <boost/filesystem.hpp>

#include <set>

namespace Log
{
  const std::string XmlFileStore::ArcSuffix = ".arc.xml";

  XmlFileStore::XmlFileStore(const std::string& path, FileSystemFacade& facade, const size_t maxLogSize, const unsigned maxArcCount)
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
    std::set<std::string> files = FileSystem.GetArcList(Path);

    if (files.size() > MaxArcCount)
    {
      std::set<std::string>::const_iterator itFile = files.begin();
      for (size_t i = 0; i < files.size() - MaxArcCount; ++i, ++itFile)
      {
        FileSystem.RemoveFile(*itFile);
      }
    }
  }
}