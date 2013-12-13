#include "logger.h"
#include "memory_store.h"

#include <gtest/gtest.h>

#include <sstream>

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

  Log::MemoryStore store;
  Log::Logger log(store);

  log.Write("cat1", "msg1");
  log.Write("cat1", "msg2");
  log.Write("cat3", "msg3");
  log.Write("cat4", "msg4");
  boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));

  Log::EventList events = store.Find("cat1");

  for(Log::EventList::const_iterator it = events.begin(), end = events.end(); it != end; ++it)
    std::cout << (*it)->Message << std::endl;

  return 0;
}
