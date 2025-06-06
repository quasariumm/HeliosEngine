#include "EditorSettings.h"

#include "Core/FileTools.h"
#include "Debugger/Debugger.h"

namespace Engine
{
EditorSettingsData EditorSettings::m_editorSettings = {};
bool EditorSettings::m_editorSettingsOpen = false;

bool EditorSettings::Save()
{
    std::wofstream file;
    file.open("EditorSettings.txt");
    if (!file.is_open())
    {
        DebugLog(LogSeverity::SEVERE, L"Failed to save editor settings");
        return false;
    }
    // Save all variables in the struct
    file << "CloseOnEscape = " << std::to_wstring(m_editorSettings.m_closeOnEscape) << std::endl;
    DebugLog(LogSeverity::DONE, L"Successfully saved editor settings");
    return true;
}

bool EditorSettings::Load()
{
    std::wifstream file;
    file.open("EditorSettings.txt");
    if (!file.is_open())
    {
        DebugLog(LogSeverity::SEVERE, L"Failed to load editor settings");
        return false;
    }
    std::wstring line;
    // Load all variable in the struct
    while (std::getline(file, line))
    {
        if (IsToken(line, L"CloseOnEscape")) m_editorSettings.m_closeOnEscape = stoi(TokenValue(line));
    }

    DebugLog(LogSeverity::DONE, L"Successfully loaded editor settings");
    return true;
}

void EditorSettings::DrawWindow()
{
    // ReSharper disable once CppDFAConstantConditions
    if (!m_editorSettingsOpen) return;

    // ReSharper disable once CppDFAUnreachableCode
    ImGui::Begin("Editor Settings");

    if (ImGui::BeginTabBar("Editor Settings"))
    {
        if (ImGui::BeginTabItem("Shortcuts"))
        {
            ImGui::Checkbox("Close editor on " ICON_KEYBOARD_ESC, &m_editorSettings.m_closeOnEscape);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::Separator();

    if (ImGui::Button(ICON_CONTENT_SAVE" Save")) Save();
    ImGui::SameLine();
    if (ImGui::Button(ICON_CLOSE_BOX" Close")) CloseWindow();

    ImGui::End();
}
}
