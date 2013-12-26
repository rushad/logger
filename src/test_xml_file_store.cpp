#include "logger.h"
#include "fake_file_system.h"
#include "xml_file_store.h"

#include <gtest/gtest.h>

namespace Log
{
  namespace Test
  {
    class TestXmlFileStore : public ::testing::Test
    {
    protected:
      Event CreateTestEvent() const
      {
        Event theEvent;
        theEvent.Verb = VERB_INFO;
        theEvent.Time = TimeGen.Now();
        theEvent.Category = "category";
        theEvent.Message = "message";
        theEvent.Tags["first"] = "First tag";
        theEvent.Tags["second"] = "Second tag";
        theEvent.Tags["third"] = "Third tag";
        return theEvent;
      }

      const static std::string Header;
      TimeGenerator TimeGen;
    };

    const std::string TestXmlFileStore::Header = "<?xml version=\"1.0\"?>\n<log>\n";

    TEST_F(TestXmlFileStore, CtorShouldCreateAbsentPath)
    {
      FakeFileSystem facade;
      XmlFileStore store("log", facade);

      EXPECT_EQ(1, facade.CreateDirCallCount);
      EXPECT_EQ("log", facade.LastCreatedDir);

      XmlFileStore store1("log", facade);
      EXPECT_EQ(1, facade.CreateDirCallCount);
    }

    TEST_F(TestXmlFileStore, CtorShouldCreateFileIfNotExist)
    {
      FakeFileSystem facade;
      XmlFileStore store("log", facade);

      EXPECT_TRUE(store.GetStream().good());
      EXPECT_EQ(1, facade.CreateFileCallCount);
      EXPECT_EQ("log/log.xml", facade.Name);
    }

    TEST_F(TestXmlFileStore, CtorShouldWriteHeaderInNewlyCreatedFile)
    {
      FakeFileSystem facade;
      XmlFileStore store("log", facade);

      std::ostringstream& stream = static_cast<std::ostringstream&>(store.GetStream());
      EXPECT_EQ(Header, stream.str());
    }

    TEST_F(TestXmlFileStore, CreateFileErrorShouldThrowException)
    {
      FakeFileSystem facade;
      facade.ThrowOnCreate = true;
      
      EXPECT_THROW(XmlFileStore store("log", facade), std::exception);
      
    }

    TEST_F(TestXmlFileStore, LogShouldWriteToFile)
    {
      FakeFileSystem facade;
      XmlFileStore store("log", facade);

      std::ostringstream& stream = static_cast<std::ostringstream&>(store.GetStream());
      std::size_t l1 = stream.str().size();

      store.Add(CreateTestEvent());

      std::size_t l2 = stream.str().size();
      EXPECT_GT(l2 - l1, (size_t)0);
    }

    TEST_F(TestXmlFileStore, TestWrittenToFile)
    {
      FakeFileSystem facade;
      XmlFileStore store("log", facade);
      std::ostringstream& stream = static_cast<std::ostringstream&>(store.GetStream());

      Event theEvent = CreateTestEvent();
      store.Add(theEvent);

      std::string expected = Header +
        "  <event time=\"" + theEvent.Time.ToString() + "\" category=\"category\" verbosity=\"info\">\n"
        "    <first>First tag</first>\n"
        "    <second>Second tag</second>\n"
        "    <third>Third tag</third>\n"
        "    message\n"
        "  </event>\n";

      EXPECT_EQ(expected, stream.str());
    }

    TEST_F(TestXmlFileStore, RotateShouldCreateNewStream)
    {
      const unsigned maxLogSize = 1024;
      FakeFileSystem facade;
      XmlFileStore store("log", facade, maxLogSize);

      Event theEvent = CreateTestEvent();

      facade.OpenFileCallCount = 0;
      facade.CreateFileCallCount = 0;
      facade.RenameFileCallCount = 0;

      const std::string strEvent = 
        "  <event time=\"" + theEvent.Time.ToString() + "\" category=\"category\" verbosity=\"info\">\n"
        "    <first>First tag</first>\n"
        "    <second>Second tag</second>\n"
        "    <third>Third tag</third>\n"
        "    message\n"
        "  </event>\n";

      std::ostream& stream1 = store.GetStream();
      for(size_t l = Header.size(); l < maxLogSize; l += strEvent.size())
      {
        store.Add(theEvent);
      }
      std::ostream& stream2 = store.GetStream();

      EXPECT_EQ(0, facade.OpenFileCallCount);
      EXPECT_EQ(1, facade.CreateFileCallCount);
      EXPECT_EQ(1, facade.RenameFileCallCount);
    }

    TEST_F(TestXmlFileStore, RotateShouldAddCloseTag)
    {
      FakeFileSystem facade;
      XmlFileStore store("log", facade);

      store.Rotate();

      EXPECT_EQ(Header + "</log>\n", facade.LastStreamContent);
    }

    TEST_F(TestXmlFileStore, RotateShouldRenameFile)
    {
      FakeFileSystem facade;
      XmlFileStore store("log", facade);

      store.Rotate();
      EXPECT_EQ(1, facade.RenameFileCallCount);
      EXPECT_EQ("log/" + store.GetLastTime().ToString() + ".arc.xml", facade.RotatedName);
    }

    TEST_F(TestXmlFileStore, RotateShouldRemoveOldFiles)
    {
      const unsigned maxArcCount = 10;
      FakeFileSystem facade;
      XmlFileStore store("log", facade, 1024, maxArcCount);

      store.Rotate();
      std::string arcName = facade.RotatedName;

      for(int i = 0; i < maxArcCount; ++i)
        store.Rotate();

      EXPECT_EQ(1, facade.RemovedFilesCount);
      EXPECT_EQ(arcName, facade.LastRemovedFile);
    }
  }
}