#include "file_system.h"
#include "logger.h"
#include "xml_file_store.h"

#include <gtest/gtest.h>

bool CheckOptions(int argc, char** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string opt(argv[i]);
    if(opt == "--gtest")
    {
      std::cout << "Running tests..." << std::endl;
      testing::InitGoogleTest(&argc, argv);
      RUN_ALL_TESTS();
      return false;
    }
  }

  return true;
}

GTEST_API_ int main(int argc, char** argv) 
{
  setlocale(LC_ALL, "RUS");

  if(!CheckOptions(argc, argv))
  {
    return -1;
  }

  Log::FileSystem fs;
  Log::XmlFileStore store("log", fs, 1024);
  Log::Logger log(store, Log::VERB_INFO);

  Log::MapTags tags;
  tags["first"] = "First tag";
  tags["second"] = "Second tag";
  tags["third"] = "Third tag";
  log.Write(Log::VERB_INFO, "cat1", "сообщение", tags);
  log.Write(Log::VERB_INFO, "cat1", "msg2", tags);
  log.Write(Log::VERB_INFO, "cat3", "msg3");
  log.Write(Log::VERB_INFO, "cat4", "msg4", tags);

  log.WaitForFlush();
/*
//  Log::MemoryStore store;
  try
  {
//    Log::XmlFile file(10*1024, "log");
//    Log::FakeXmlFile file;
    Log::XmlFileStore store("log");
    Log::Logger log(store, Log::VERB_INFO);

    Log::MapTags tags;
    tags["first"] = "First tag";
    tags["second"] = "Second tag";
    tags["third"] = "Third tag";
    log.Write(Log::VERB_INFO, "cat1", "сообщение", tags);
    log.Write(Log::VERB_INFO, "cat1", "msg2", tags);
    log.Write(Log::VERB_INFO, "cat3", "msg3");
    log.Write(Log::VERB_INFO, "cat4", "msg4", tags);

    log.WaitForFlush();
  }
  catch(const std::exception& e)
  {
    std::cout << e.what() << std::endl;
  }
*/
/*
  Log::EventList events = store.Find("cat1");

  for(Log::EventList::const_iterator it = events.begin(), end = events.end(); it != end; ++it)
    std::cout << (*it)->Message << std::endl;
*/
  return 0;
}
