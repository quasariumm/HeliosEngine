#include "Debugger.h"

#include <bits/fs_fwd.h>
#include <bits/fs_path.h>

#include "Math/Vector.h"
#include "Core/IconsFA.h"

namespace Engine
{
std::vector<Logger::Log> Logger::g_logs = {};
std::vector<Logger::Watch> Logger::g_watchList = {};
std::string Logger::m_totalLog;

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
    // Options
    static bool lockToBottom = true, showTimestamp = true, showSource = true, showFullPath = false;

    // Severities
    static bool showInfo = true, showWarn = true, showError = true, showDone = true;

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            ImGui::MenuItem("Lock to bottom", nullptr, &lockToBottom);
            ImGui::MenuItem("Show timestamp", nullptr, &showTimestamp);
            ImGui::MenuItem("Show source", nullptr, &showSource);
            ImGui::MenuItem("Show full path", nullptr, &showFullPath, showSource);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Severity"))
        {
            ImGui::MenuItem(ICON_FA_INFO" Info", nullptr, &showInfo);
            ImGui::MenuItem(ICON_FA_TRIANGLE_EXCLAMATION " Warnings", nullptr, &showWarn);
            ImGui::MenuItem(ICON_FA_CIRCLE_EXCLAMATION " Errors", nullptr, &showError);
            ImGui::MenuItem(ICON_FA_CHECK " Completion", nullptr, &showDone);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    for (const Logger::Log& log : Logger::g_logs)
    {
        if (log.type == LogSeverity::INFO && !showInfo) continue;
        if (log.type == LogSeverity::WARNING && !showWarn) continue;
        if (log.type == LogSeverity::ERROR && !showError) continue;
        if (log.type == LogSeverity::DONE && !showDone) continue;


        ImGui::TextColored({1,1,1,0.5f}, ICON_FA_ARROW_RIGHT);
        float spacing = 30;
        ImGui::SameLine(spacing);


        if (showSource)
        {
            ImGui::Text(ICON_FA_CODE);
            std::filesystem::path file = std::filesystem::path(log.source.file_name()).make_preferred();
            const std::filesystem::path origin = std::filesystem::current_path().make_preferred();

            if (!showFullPath)
                file = file.lexically_relative(origin);

            ImGui::SetItemTooltip(ICON_FA_FILE_CODE " %s\n" ICON_FA_BARS " %o:%o\n" ICON_FA_CODE " %s", file.string().c_str(), log.source.line(), log.source.column(), log.source.function_name());
            spacing += 25;
            ImGui::SameLine(spacing);
        }

        if (showTimestamp)
        {
            char timestamp[48];
            strftime(timestamp, 48, "%H:%M:%S", localtime(&log.timestamp));
            ImGui::Text(timestamp);
            spacing += 65;
            ImGui::SameLine(spacing);
        }


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

        spacing += 30;
        ImGui::SameLine(spacing);
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
