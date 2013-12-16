#include "logger.h"
#include "xml_store.h"

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

//  Log::MemoryStore store;
  Log::XmlStore store("log.xml");
  Log::Logger log(store, Log::VERB_INFO);

  log.Write(Log::VERB_INFO, "cat1", "сообщение");
  log.Write(Log::VERB_INFO, "cat1", "msg2");
  log.Write(Log::VERB_INFO, "cat3", "msg3");
  log.Write(Log::VERB_INFO, "cat4", "msg4");

  log.WaitForFlush();
/*
  Log::EventList events = store.Find("cat1");

  for(Log::EventList::const_iterator it = events.begin(), end = events.end(); it != end; ++it)
    std::cout << (*it)->Message << std::endl;
*/
  return 0;
}
