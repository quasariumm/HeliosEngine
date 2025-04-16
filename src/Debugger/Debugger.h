#pragma once

#include "Editor/EditorInterface.h"

namespace Engine
{

enum class LogSeverity
{
    INFO,
    WARNING,
    ERROR,
    DONE
};

inline std::string LogSeverityString(LogSeverity severity)
{
    switch (severity) {
    default:
    case LogSeverity::INFO: return "INFO";
    case LogSeverity::WARNING: return "WARNING";
    case LogSeverity::ERROR: return "ERROR";
    case LogSeverity::DONE: return "DONE";
    }
}

inline std::string TimeStamp(time_t time)
{
    char timestamp[32];
    strftime(timestamp, 32, "%H:%M:%S", localtime(&time));
    return {timestamp};
}

class Logger
{
public:
    struct Log;
    struct Watch;

    static std::vector<Log> g_logs;
    static std::vector<Watch> g_watchList;

    /// Log data with type, message and a level (Level 0 means always visible, while a higher level can be ignored)
    struct Log
    {
        explicit Log(const LogSeverity type, std::string message, const int level = 0, std::source_location location = std::source_location::current()) :
        type(type),
        message(std::move(message)),
        level(level),
        timestamp(time(nullptr)),
        source(location)
        { }

        LogSeverity type = LogSeverity::INFO;
        time_t timestamp = 0;
        int level = 0;
        std::string message;
        std::source_location source;

        std::string FileText() { return std::format("[{}]  \n[{}] [{}]  \n[{}] [{}]  \n**{}**  \n---\n",
            source.file_name(),
            "Line: " + std::to_string(source.line()) + ":" + std::to_string(source.column()),
            source.function_name(), TimeStamp(timestamp), LogSeverityString(type), message); }
    };

    struct Watch
    {
        template <class T>
        explicit Watch(std::string name, T* value) : name(std::move(name)), type(typeid(T)), var(value) { }

        const std::string name;
        const std::type_info& type;
        void* var;
    };

    static std::string m_totalLog;

    static void ExportLog()
    {
        if (!ProjectLoaded())
        {
            std::cout << "Could not export log, no project was loaded";
            return;
        }

        std::ofstream file;
        file.open(ProjectFolder().append("Latest-Log.md"), std::ofstream::out | std::ofstream::trunc);
        if (!file.is_open())
        {
            std::cout << "Could not export log, failed to write log";
            return;
        }
        file << m_totalLog;
        file.close();
    }
};

/// Add a log to the debug viewer
inline void DebugLog(const LogSeverity type, const std::string& message, const int level = 0, std::source_location location = std::source_location::current())
{
    Logger::g_logs.emplace_back(type, message, level, location);
    Logger::m_totalLog += Logger::g_logs.back().FileText();
}

/// Add a value to the value debugger. Can be modified from the menu
template <class T>
static void DebugWatch(const std::string& name, T* value) { Logger::g_watchList.emplace_back(name, value); };

class Debugger final : public EditorInterface
{
public:
    Debugger(): EditorInterface("Debugger") {}
    ~Debugger() override { Logger::ExportLog(); };

    void DrawInterface() override;

private:
    static void DrawLogs();
    static void DrawWatchList();

    int m_debugLevel = 5;
};
}
