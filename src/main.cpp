#include "logger.h"

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

  std::ostringstream oss;
  Log::LoggerPtr log = Log::CreateLogger(Log::BackendPtr(new Log::BackendStream(oss)));
  log->write("Hello world!");
  std::cout << oss.str();
  return 0;
}
