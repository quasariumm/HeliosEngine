#pragma once
#include <iostream>
#include <ostream>
#include <string>
#include <typeinfo>
#include <utility>

#include "Editor/EditorInterface.h"

namespace Engine
{

enum LogType
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
        explicit Log(const LogType type, std::string message, const int level = 0) : type(type), message(std::move(message)), level(level) { }

        LogType type = INFO;
        int level = 0;
        std::string message;
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
static void DebugLog(const LogType type, const std::string& message, const int level = 0)
{
    Logger::g_logs.emplace_back(type, message, level);
    std::cout << Logger::g_logs.size() << std::endl;
}

/// Add a value to the value debugger. Can be modified from the menu
template <class T>
static void DebugWatch(const std::string& name, T* value) { Logger::g_watchList.emplace_back(name, value); };

class Debugger final : public EditorInterface
{
    void DrawInterface() override;

    static void DrawLogs();
    static void DrawWatchList();

    int m_debugLevel = 5;
};
}
