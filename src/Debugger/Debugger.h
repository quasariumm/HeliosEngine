#pragma once
#include <iostream>
#include <source_location>
#include <string>
#include <typeinfo>
#include <utility>

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
    };

    struct Watch
    {
        template <class T>
        explicit Watch(std::string name, T* value) : name(std::move(name)), type(typeid(T)), var(value) { }

        const std::string name;
        const std::type_info& type;
        void* var;
    };
};

/// Add a log to the debug viewer
inline void DebugLog(const LogSeverity type, const std::string& message, const int level = 0, std::source_location location = std::source_location::current())
{
    Logger::g_logs.emplace_back(type, message, level, location);
}

/// Add a value to the value debugger. Can be modified from the menu
template <class T>
static void DebugWatch(const std::string& name, T* value) { Logger::g_watchList.emplace_back(name, value); };

class Debugger final : public EditorInterface
{
public:
    Debugger(): EditorInterface("Debugger") {}
    void DrawInterface() override;

private:
    static void DrawLogs();
    static void DrawWatchList();

    int m_debugLevel = 5;
};
}
