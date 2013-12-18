#include "xml_file_store.h"

namespace Log
{
  XmlFile::XmlFile(const std::string& dirName)
    : DirName(dirName)
  {
    FileStream.open((dirName + "/" + "log.xml").c_str());
    if (!FileStream.is_open())
      throw std::exception("XmlFile::XmlFile() failed");
    FileStream << "<?xml version=\"1.0\"?>" << std::endl;
    FileStream << "<log>" << std::endl;
  }

  XmlFile::~XmlFile()
  {
    FileStream << "</log>" << std::endl;
    FileStream.close();
  }

  void XmlFile::Write(const std::string& str)
  {
    boost::lock_guard<boost::mutex> locker(LockFileStream);
    FileStream << str;
  }

  XmlFileStore::XmlFileStore(XmlFileInterface* file)
    : File(file)
  {
  }

  void FakeXmlFile::Write(const std::string& str)
  {
    Records.push_back(str);
  }

  std::string FakeXmlFile::GetRecord(unsigned index) const
  {
    if(index >= Records.size())
      throw std::exception("Index is out of range");
    return Records[index];
  }

  void XmlFileStore::Add(const EventPtr& theEvent)
  {
    std::string str = "  <event";
    str += " time=\"" + theEvent->Time.ToString() + "\"";
    str += " category=\"" + theEvent->Category + "\"";
    str += " verbosity=\"" + Verb2Str(theEvent->Verb) + "\">\n";

    for(MapTags::const_iterator it = theEvent->Tags.begin(), end = theEvent->Tags.end(); it != end; ++it)
    {
      str += "    <" + it->first + ">" + it->second + "</" + it->first + ">\n";
    }

    str += "    " + theEvent->Message + "\n";
    str += "  </event>\n";

    File->Write(str);
  }
}
