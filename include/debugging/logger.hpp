#pragma once

#define LOGLEVEL_MINIMUM    0
#define LOGLEVEL_ENGINE     3
#define LOGLEVEL_PROJECT    10

#define LOGLEVEL_DEFAULT LOGLEVEL_ENGINE

#include "core/systems.hpp"

namespace Engine::Log
{

enum class LogType
{
    INFO = 0,
    WARNING = 1,
    ERROR = 2,
    DONE = 3
};

struct Log
{
    std::string message;
    LogType type;
    int level;
    std::source_location location;
};

class LogHandler
{
public:
    void RegisterLog(const Log& log) { logs.push_back(log); }

    std::vector<Log>& GetLogs() { return logs; }

    // TODO(Quillan): Add a way to export the log to a file

private:
    std::vector<Log> logs = {};
};

inline void Info(const std::string& message, const int& level = LOGLEVEL_DEFAULT, const std::source_location& location = std::source_location::current())
{ Systems::GetLogHandler()->RegisterLog(Log(message, LogType::INFO, level, location)); }
inline void Warn(const std::string& message, const int& level = LOGLEVEL_DEFAULT, const std::source_location& location = std::source_location::current())
{ Systems::GetLogHandler()->RegisterLog(Log(message, LogType::WARNING, level, location)); }
inline void Error(const std::string& message, const int& level = LOGLEVEL_DEFAULT, const std::source_location& location = std::source_location::current())
{ Systems::GetLogHandler()->RegisterLog(Log(message, LogType::ERROR, level, location)); }
inline void Done(const std::string& message, const int& level = LOGLEVEL_DEFAULT, const std::source_location& location = std::source_location::current())
{ Systems::GetLogHandler()->RegisterLog(Log(message, LogType::DONE, level, location)); }

}
