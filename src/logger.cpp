#include "logger.h"

namespace Log
{
  Logger::Logger(Store& store)
    : TheStore(store)
  {
  }

  void Logger::Write(const std::string& category, const std::string& message)
  {
    Event theEvent;
    theEvent.Message = message;
    theEvent.Category = category;
    TheStore.Add(theEvent);
  }
}