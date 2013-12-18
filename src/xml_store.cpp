#include "xml_store.h"

namespace Log
{
  XmlStore::XmlStore(const std::string& dirName)
  {
    File.open((dirName + "/" + "log.xml").c_str());
    if (!File.is_open())
      throw std::exception("XmlStore::XmlStore() failed");
    File << "<?xml version=\"1.0\"?>" << std::endl;
    File << "<log>" << std::endl;
  }

  XmlStore::~XmlStore()
  {
    File << "</log>" << std::endl;
    File.close();
  }

  void XmlStore::Add(const EventPtr& theEvent)
  {
    boost::lock_guard<boost::mutex> locker(LockFile);
    File << "  <event";
    File << " time=\"" << theEvent->Time.ToString() << "\"";
    File << " category=\"" << theEvent->Category << "\"";
    File << " verbosity=\"" << Verb2Str(theEvent->Verb) << "\">" << std::endl;

    for(MapTags::const_iterator it = theEvent->Tags.begin(), end = theEvent->Tags.end(); it != end; ++it)
    {
      File << "    <" << it->first << ">" << it->second << "</" << it->first << ">" << std::endl;
    }

    File << "    " << theEvent->Message << std::endl;
    File << "  </event>" << std::endl;
  }
}
