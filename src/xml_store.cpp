#include "xml_store.h"

namespace Log
{
  XmlStore::XmlStore(const std::string& fileName)
  {
    File.open(fileName.c_str());
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
    File << "  <event" << std::endl;
    File << "    time=\"" << theEvent->Time << "." << theEvent->Tune << "\"" << std::endl;
    File << "    category=\"" << theEvent->Category << "\"" << std::endl;
    File << "    verbosity=\"" << Verb2Str(theEvent->Verb) << "\">" << std::endl;
    File << "  " << theEvent->Message << "</event>" << std::endl;
  }
}
