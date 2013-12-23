#include "logger.h"
#include "fake_file_system.h"
#include "xml_file_store.h"

#include <gtest/gtest.h>

namespace Log
{
  namespace Test
  {
    class FakeUniqueTime : public UniqueTime
    {
    protected:
      virtual boost::posix_time::ptime GetClockTime() const
      {
        return FakeTime;
      }

    private:
      const static boost::posix_time::ptime FakeTime;
    };

    class TestXmlFileStore : public ::testing::Test
    {
    protected:
      Event CreateTestEvent() const
      {
        Event theEvent;
        theEvent.Verb = VERB_INFO;
        theEvent.Time = FakeUniqueTime();
        theEvent.Category = "category";
        theEvent.Message = "message";
        theEvent.Tags["first"] = "First tag";
        theEvent.Tags["second"] = "Second tag";
        theEvent.Tags["third"] = "Third tag";
        return theEvent;
      }

      const static std::string Header;
    };

    const std::string TestXmlFileStore::Header = "<?xml version=\"1.0\"?>\n<log>\n";

    const boost::posix_time::ptime FakeUniqueTime::FakeTime = boost::posix_time::microsec_clock::universal_time();

    TEST_F(TestXmlFileStore, CtorShouldCreateAbsentPath)
    {
      FakeFileSystem facade;
      XmlFileStore store("log", facade);

      ASSERT_EQ(1, facade.CreateDirCallCount);
      ASSERT_EQ("log", facade.LastCreatedDir);

      XmlFileStore store1("log", facade);
      ASSERT_EQ(1, facade.CreateDirCallCount);
    }

    TEST_F(TestXmlFileStore, CtorShouldCreateFileIfNotExist)
    {
      FakeFileSystem facade;
      XmlFileStore store("log", facade);

      ASSERT_TRUE(store.GetStream().good());
      ASSERT_EQ(1, facade.CreateCallCount);
      ASSERT_EQ("log/log.xml", facade.Name);
    }

    TEST_F(TestXmlFileStore, CtorShouldWriteHeaderInNewlyCreatedFile)
    {
      FakeFileSystem facade;
      XmlFileStore store("log", facade);

      std::ostringstream& stream = static_cast<std::ostringstream&>(store.GetStream());
      ASSERT_EQ(Header, stream.str());
    }

    TEST_F(TestXmlFileStore, DISABLED_OpenFileErrorShouldThrowException)
    {
    }

    TEST_F(TestXmlFileStore, LogShouldWriteToFile)
    {
      FakeFileSystem facade;
      XmlFileStore store("log", facade);

      std::ostringstream& stream = static_cast<std::ostringstream&>(store.GetStream());
      std::size_t l1 = stream.str().size();

      store.Add(CreateTestEvent());

      std::size_t l2 = stream.str().size();
      ASSERT_GT(l2 - l1, (size_t)0);
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

      ASSERT_EQ(expected, stream.str());
    }

    TEST_F(TestXmlFileStore, RotateShouldCreateNewStream)
    {
      const unsigned maxLogSize = 10*1024;
      FakeFileSystem facade;
      XmlFileStore store("log", facade, maxLogSize);

      Event theEvent = CreateTestEvent();

      facade.OpenCallCount = 0;
      facade.CreateCallCount = 0;

      std::ostream& stream1 = store.GetStream();
      while (stream1.tellp() < maxLogSize)
      {
        store.Add(theEvent);
      }
      std::ostream& stream2 = store.GetStream();

      ASSERT_NE(&stream1, &stream2);
      ASSERT_EQ(0, facade.OpenCallCount);
      ASSERT_EQ(1, facade.CreateCallCount);
    }

    TEST_F(TestXmlFileStore, RotateShouldAddCloseTag)
    {
      FakeFileSystem facade;
      XmlFileStore store("log", facade);

      store.Rotate();

      ASSERT_EQ(Header + "</log>\n", facade.LastStreamContent);
    }

    TEST_F(TestXmlFileStore, RotateShouldRenameFile)
    {
      FakeFileSystem facade;
      XmlFileStore store("log", facade);

      store.Rotate();


    }
  }
}