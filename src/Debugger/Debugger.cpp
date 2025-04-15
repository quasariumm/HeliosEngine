#include "Debugger.h"

#include "Math/Vector.h"

namespace Engine
{

std::vector<Logger::Log> Logger::g_logs = {};
std::vector<Logger::Watch> Logger::g_watchList = {};

void Debugger::DrawInterface()
{
    ImGui::Begin("Debugger");

    if (!ImGui::BeginTabBar("DebugTab"))
        return;

    if (ImGui::BeginTabItem("Logging"))
    {
        DrawLogs();
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Watch List"))
    {
        DrawWatchList();
        ImGui::EndTabItem();
    }

    ImGui::EndTabBar();

    ImGui::End();
}

void Debugger::DrawLogs()
{
    for (const Logger::Log& log : Logger::g_logs)
    {
        ImGui::Text(log.message.c_str());
    }
}

void Debugger::DrawWatchList()
{
    for (Logger::Watch& watch : Logger::g_watchList)
    {
        if (watch.type == typeid(int))
            ImGui::InputInt(watch.name.c_str(), (int*)watch.var);
        else if (watch.type == typeid(float))
            ImGui::InputFloat(watch.name.c_str(), (float*)watch.var);
        else if (watch.type == typeid(vec2))
            ImGui::InputFloat2(watch.name.c_str(), ((vec2*)watch.var)->cell);
        else if (watch.type == typeid(vec3))
            ImGui::InputFloat2(watch.name.c_str(), ((vec3*)watch.var)->cell);
        else
            ImGui::Text("Watch '%s' does not have a supported type", watch.name);
    }
}
}
