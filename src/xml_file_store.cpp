#include "xml_file_store.h"

#include <boost/filesystem.hpp>

namespace Log
{
  XmlFileStore::XmlFileStore(const std::string& path, FileSystemFacade& facade, const size_t maxLogSize)
    : Path(path)
    , FileSystem(facade)
    , MaxLogSize(maxLogSize)
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
    *Stream << "<?xml version=\"1.0\"?>\n<log>\n";
  }

  void XmlFileStore::Rotate()
  {
    *Stream << "</log>\n";
    FileSystem.OnCloseFile(*Stream);
    Stream.release();
    InitStream();
  }

  std::string XmlFileStore::GetFileName() const
  {
    return Path + "/log.xml";
  }

}