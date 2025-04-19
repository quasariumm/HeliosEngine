#include "Debugger.h"

namespace Engine
{
std::vector<Logger::Log> Logger::g_logs = {};
std::vector<Logger::Watch> Logger::g_watchList = {};
std::vector<Logger::Watch> Logger::g_tempWatchList = {};
std::string Logger::m_totalLog;

void Debugger::DrawInterface()
{
    ImGui::Begin(ICON_VIEW_LIST " Logs", nullptr, ImGuiWindowFlags_MenuBar);
    DrawLogs();
    ImGui::End();

    ImGui::Begin(ICON_EYE " Watch List");
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
        if (ImGui::BeginMenu(ICON_COG" Options"))
        {
            ImGui::MenuItem(ICON_ARROW_COLLAPSE_DOWN" Lock to bottom", nullptr, &lockToBottom);
            ImGui::MenuItem(ICON_CLOCK" Show timestamp", nullptr, &showTimestamp);
            ImGui::MenuItem(ICON_CODE_BRACES" Show source", nullptr, &showSource);
            ImGui::MenuItem(ICON_FOLDER" Show full path", nullptr, &showFullPath, showSource);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu(ICON_ALERT_BOX" Severity"))
        {
            ImGui::MenuItem(ICON_INFORMATION_VARIANT_BOX" Info", nullptr, &showInfo);
            ImGui::MenuItem(ICON_ALERT " Warnings", nullptr, &showWarn);
            ImGui::MenuItem(ICON_ALERT_OCTAGON " Errors", nullptr, &showError);
            ImGui::MenuItem(ICON_CHECK_BOLD " Completion", nullptr, &showDone);
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem(ICON_CLOSE_BOX" Clear"))
            Logger::g_logs.clear();
        ImGui::EndMenuBar();
    }

    for (const Logger::Log& log : Logger::g_logs)
    {
        if (log.type == LogSeverity::INFO && !showInfo) continue;
        if (log.type == LogSeverity::WARNING && !showWarn) continue;
        if (log.type == LogSeverity::ERROR && !showError) continue;
        if (log.type == LogSeverity::DONE && !showDone) continue;


        ImGui::TextColored({1,1,1,0.5f}, ICON_CHEVRON_RIGHT);
        float spacing = 30;
        ImGui::SameLine(spacing);


        if (showSource)
        {
            ImGui::Text(ICON_CODE_BLOCK_BRACES);
            std::filesystem::path file = std::filesystem::path(log.source.file_name()).make_preferred();
            const std::filesystem::path origin = std::filesystem::current_path().make_preferred();

            if (!showFullPath)
                file = file.lexically_relative(origin);

            ImGui::SetItemTooltip(ICON_FILE_CODE " %s\n" ICON_FORMAT_LIST_NUMBERED " %o:%o\n" ICON_CODE_BRACES " %s",
                file.string().c_str(), log.source.line(), log.source.column(), log.source.function_name());
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
        case LogSeverity::INFO: ImGui::TextColored({0.3f, 0.5f, 0.8f, 1.0f}, ICON_INFORMATION_VARIANT_BOX);
            break;
        case LogSeverity::WARNING: ImGui::TextColored({1.0f, 1.0f, 0, 1.0f}, ICON_ALERT);
            break;
        case LogSeverity::ERROR: ImGui::TextColored({1.0f, 0, 0, 1.0f}, ICON_ALERT_OCTAGON);
            break;
        case LogSeverity::DONE: ImGui::TextColored({0, 0.5f, 0, 1.0f}, ICON_CHECK_BOLD);
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
            ImGui::InputFloat3(watch.name.c_str(), ((vec3*)watch.var)->cell);
        else
            ImGui::Text("Watch '%s' does not have a supported type", watch.name);
    }

    ImGui::BeginDisabled();
    for (Logger::Watch& watch : Logger::g_tempWatchList)
    {
        if (watch.type == typeid(int))
            ImGui::InputInt(watch.name.c_str(), (int*)watch.var);
        else if (watch.type == typeid(float))
            ImGui::InputFloat(watch.name.c_str(), (float*)watch.var);
        else if (watch.type == typeid(vec2))
            ImGui::InputFloat2(watch.name.c_str(), ((vec2*)watch.var)->cell);
        else if (watch.type == typeid(vec3))
            ImGui::InputFloat3(watch.name.c_str(), ((vec3*)watch.var)->cell);
        else
            ImGui::Text("Watch '%s' does not have a supported type", watch.name);
    }
    ImGui::EndDisabled();

    Logger::g_tempWatchList.clear();

}

REGISTER_EDITOR_INTERFACE(Debugger);

}
