#include "xml_file_store.h"

namespace Log
{
  XmlFile::XmlFile(const std::string& dirName, const unsigned maxSize)
    : DirName(dirName)
    , MaxSize(maxSize)
  {
    FileStream.open((DirName + "/log.xml").c_str(), std::ofstream::ate | std::ofstream::app);
    if (!FileStream.is_open())
      throw std::exception("XmlFile::XmlFile() failed");
    if (!FileStream.tellp())
    {
      FileStream << "<?xml version=\"1.0\"?>" << std::endl;
      FileStream << "<log>" << std::endl;
    }
  }

  XmlFile::~XmlFile()
  {
    FileStream.close();
  }

  void XmlFile::Write(const std::string& str)
  {
    boost::lock_guard<boost::mutex> locker(LockFileStream);
    FileStream << str;
    if(FileStream.tellp() > (std::streampos)(MaxSize * 1024))
      Rotate();
  }

  void XmlFile::Rotate()
  {
    FileStream << "</log>" << std::endl;
    FileStream.close();
    
    rename((DirName + "/log.xml").c_str(), (DirName + "/" + UniqueTime().ToString() + ".xml").c_str());
    
    FileStream.open((DirName + "/log.xml").c_str(), std::ofstream::trunc);
    if (!FileStream.is_open())
      throw std::exception(("Create file " + DirName + "/log.xml failed").c_str());

    FileStream << "<?xml version=\"1.0\"?>" << std::endl;
    FileStream << "<log>" << std::endl;
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
