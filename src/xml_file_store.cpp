#include "xml_file_store.h"

namespace Log
{
  XmlFileLogic::XmlFileLogic(unsigned maxLogSize)
    : MaxLogSize(maxLogSize)
    , Written(0)
  {
//    OpenFile();
  }

  XmlFileLogic::~XmlFileLogic()
  {
  }

  void XmlFileLogic::Write(const std::string& str)
  {
    *StreamPtr << str;
    Written += (unsigned)str.size();
    if(Written > MaxLogSize)
      Rotate();
  }

  XmlFile::XmlFile(const unsigned maxLogSize, const std::string& dirName)
    : XmlFileLogic(maxLogSize)
    , DirName(dirName)
  {
    std::auto_ptr<std::ofstream> streamPtr(new std::ofstream);
    streamPtr->open((DirName + "/log.xml").c_str(), std::ofstream::ate | std::ofstream::app);
    if (!streamPtr->is_open())
      throw std::exception("XmlFile::OpenFile() failed");
    if (!streamPtr->tellp())
    {
      *streamPtr << "<?xml version=\"1.0\"?>" << std::endl;
      *streamPtr << "<log>" << std::endl;
    }
    Written = streamPtr->tellp();
    StreamPtr = streamPtr;
  }
/*
  void XmlFile::OpenFile()
  {
    std::auto_ptr<std::ofstream> streamPtr(new std::ofstream);
    streamPtr->open((DirName + "/log.xml").c_str(), std::ofstream::ate | std::ofstream::app);
    if (!streamPtr->is_open())
      throw std::exception("XmlFile::OpenFile() failed");
    if (!streamPtr->tellp())
    {
      *streamPtr << "<?xml version=\"1.0\"?>" << std::endl;
      *streamPtr << "<log>" << std::endl;
    }
    StreamPtr = streamPtr;
  }
*/
  unsigned XmlFile::GetFileSize()
  {
    std::ofstream* stream = static_cast<std::ofstream*>(StreamPtr.get());
    return stream->tellp();
  }

  XmlFile::~XmlFile()
  {
    std::ofstream* stream = static_cast<std::ofstream*>(StreamPtr.get());
    stream->close();
  }

  void XmlFile::Rotate()
  {
    std::ofstream* stream = static_cast<std::ofstream*>(StreamPtr.get());
    *stream << "</log>" << std::endl;
    stream->close();
    
    rename((DirName + "/log.xml").c_str(), (DirName + "/" + UniqueTime().ToString() + ".xml").c_str());
    
    stream->open((DirName + "/log.xml").c_str(), std::ofstream::trunc);
    if (!stream->is_open())
      throw std::exception(("Create file " + DirName + "/log.xml failed").c_str());

    *stream << "<?xml version=\"1.0\"?>" << std::endl;
    *stream << "<log>" << std::endl;
    Written = stream->tellp();
  }
/*
  FakeXmlFile::FakeXmlFile(const std::string& dirName, const unsigned maxSize)
    : MaxSize(maxSize)
  {
  }

  void FakeXmlFile::Write(const std::string& str)
  {
    Records.push_back(str);
    TotalWritten += (unsigned)str.size();
  }

  std::string FakeXmlFile::GetRecord(unsigned index) const
  {
    if(index >= Records.size())
      throw std::exception("Index is out of range");
    return Records[index];
  }

  unsigned FakeXmlFile::GetTotalWritten() const
  {
    return TotalWritten;
  }
*/
  XmlFileStore::XmlFileStore(XmlFileLogic* file)
    : File(file)
  {
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
