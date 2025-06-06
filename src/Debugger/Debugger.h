#pragma once

#include "Editor/EditorInterface.h"
#include "Projects/ProjectHandler.h"

namespace Engine
{

enum class LogSeverity
{
    INFO = 0,
    WARNING = 1,
    SEVERE = 2,
    DONE = 3
};

inline std::wstring LogSeverityString(LogSeverity severity)
{
    switch (severity) {
    default:
    case LogSeverity::INFO: return L"INFO";
    case LogSeverity::WARNING: return L"WARNING";
    case LogSeverity::SEVERE: return L"ERROR";
    case LogSeverity::DONE: return L"DONE";
    }
}

inline std::wstring TimeStamp(time_t time)
{
    char timestamp[32];
    strftime(timestamp, 32, "%H:%M:%S", localtime(&time));
    return STR_TO_WSTR(timestamp);
}

class Logger
{
public:
    struct Log;
    struct Watch;

    static ENGINE_API Logger* Get();

    std::vector<Log> g_logs = {};
    std::vector<Watch> g_watchList = {};
    std::vector<Watch> g_tempWatchList = {};
    std::wstring m_totalLog = {};

    /// Log data with type, message and a level (Level 0 means always visible, while a higher level can be ignored)
    struct Log
    {
        explicit Log(const LogSeverity type, std::wstring message, const int level = 0, std::source_location location = std::source_location::current()) :
        type(type),
        message(std::move(message)),
        level(level),
        timestamp(time(nullptr)),
        source(location)
        { }

        LogSeverity type = LogSeverity::INFO;
        time_t timestamp = 0;
        int level = 0;
        std::wstring message;
        std::source_location source;

        std::wstring FileText() { return std::format(L"[{}]  \n[{}] [{}]  \n[{}] [{}]  \n**{}**  \n---\n",
            STR_TO_WSTR(source.file_name()),
            L"Line: " + std::to_wstring(source.line()) + L":" + std::to_wstring(source.column()),
            STR_TO_WSTR(source.function_name()), TimeStamp(timestamp), LogSeverityString(type), message); }
    };

    struct Watch
    {
        template <class T>
        explicit Watch(std::wstring name, T* value) : name(std::move(name)), type(typeid(T)), var(value) { }

        const std::wstring name;
        const std::type_info& type;
        void* var;
    };

    static void ExportLog()
    {
        if (!ProjectHandler::ProjectLoaded())
        {
            std::cout << "Could not export log, no project was loaded\n";
            return;
        }

        std::filesystem::path folder = ProjectHandler::ProjectFolder();
        std::wstring logPath = folder.append("Latest-Log.md").generic_wstring();

        std::wofstream file;
        file.open(logPath.c_str(), std::ofstream::trunc);
        if (!file.is_open())
        {
            std::cout << "Could not export log, failed to write log\n";
            return;
        }
        file << Get()->m_totalLog;
        file.close();
    }
};

/// Add a log to the debug viewer
inline void DebugLog(const LogSeverity type, const std::wstring& message, const int level = 0,
                     std::source_location location = std::source_location::current())
{
    Logger::Get()->g_logs.emplace_back(type, message, level, location);
    Logger::Get()->m_totalLog += Logger::Get()->g_logs.back().FileText();
}

/// Add a value to the value debugger. Can be modified from the menu
template <class T>
static void DebugWatch(const std::wstring& name, T* value) { Logger::Get()->g_watchList.emplace_back(name, value); };
/// Add a value to the value debugger. Cannot be modified from the menu. But does not have to constantly exist
template <class T>
static void DebugWatchTemp(const std::wstring& name, T* value) { Logger::Get()->g_tempWatchList.emplace_back(name, value); };

class Debugger final : public EditorInterface
{
public:
    Debugger(): EditorInterface(L"Debugger") {}

    void DrawInterface() override;

private:
    static void DrawLogs();
    static void DrawWatchList();

    int m_debugLevel = 5;
};

}
