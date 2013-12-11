#include "logger.h"

namespace Log
{
  LoggerPtr CreateLogger(BackendPtr backend)
  {
    return LoggerPtr(new Logger(backend));
  }

  void Logger::write(const std::string& str) const
  {
    Backend->write(str);
  }

}