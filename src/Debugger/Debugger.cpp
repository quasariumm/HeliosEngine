#include "Debugger.h"

#include "Math/Vector.h"
#include "Core/IconsFA.h"

namespace Engine
{
std::vector<Logger::Log> Logger::g_logs = {};
std::vector<Logger::Watch> Logger::g_watchList = {};

void Debugger::DrawInterface()
{
    ImGui::Begin(ICON_FA_LIST " Logs", nullptr, ImGuiWindowFlags_MenuBar);
    DrawLogs();
    ImGui::End();

    ImGui::Begin(ICON_FA_EYE " Watch List");
    DrawWatchList();
    ImGui::End();
}

void Debugger::DrawLogs()
{
    static bool lockToBottom = true;
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            ImGui::MenuItem("Lock to bottom", nullptr, &lockToBottom);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    for (const Logger::Log& log : Logger::g_logs)
    {
        switch (log.type)
        {
        case LogSeverity::INFO: ImGui::TextColored({0.3f, 0.5f, 0.8f, 1.0f}, ICON_FA_INFO);
            break;
        case LogSeverity::WARNING: ImGui::TextColored({1.0f, 1.0f, 0, 1.0f}, ICON_FA_TRIANGLE_EXCLAMATION);
            break;
        case LogSeverity::ERROR: ImGui::TextColored({1.0f, 0, 0, 1.0f}, ICON_FA_CIRCLE_EXCLAMATION);
            break;
        case LogSeverity::DONE: ImGui::TextColored({0, 0.5f, 0, 1.0f}, ICON_FA_CHECK);
            break;
        }

        ImGui::SameLine(30);
        ImGui::Text(log.message.c_str());
    }

    static int oldSize = 0;
    if (Logger::g_logs.size() != oldSize && lockToBottom)
    {
        oldSize = (int)Logger::g_logs.size();
        ImGui::SetScrollHereY(1.0f);
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
