#include "logger.h"
#include "xml_file_store.h"

#include <gtest/gtest.h>

namespace Log
{
  namespace Test
  {
    class TestXmlFileStore : public ::testing::Test
    {
    };

    TEST_F(TestXmlFileStore, LogShouldWriteToFile)
    {
      XmlFile file(1024, "log");
      XmlFileStore store(&file);
      Logger log(store, VERB_INFO);

      unsigned l1 = file.GetFileSize();
      log.Write(VERB_INFO, "category", "message");
      log.WaitForFlush();
      unsigned l2 = file.GetFileSize();

      ASSERT_GT(l2, l1);
    }
/*
    TEST_F(TestXmlFileStore, LogShouldRotate)
    {
      unsigned maxLogSize = 1024;
      const std::string category = "category";
      const std::string message = "message";

      FakeXmlFile fakeFile("log", maxLogSize);
      XmlFileStore store(&fakeFile);
      Logger log(store, VERB_INFO, maxLogSize);

      while(fakeFile.GetTotalWritten() < maxLogSize)
        log.Write(VERB_INFO, "category", "message");

      log.WaitForFlush();

      ASSERT_EQ(1, fakeFile.GetRotations().size());
    }
*/
  }
}