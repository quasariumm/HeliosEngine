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
    file << "ForceLoadProject = " << std::to_wstring(m_editorSettings.m_forceLoadProject) << std::endl;
	file << "ForceLoadProjectPath = " << m_editorSettings.m_forceLoadProjectPath << std::endl;
	file << "ForceLoadScenePath = " << m_editorSettings.m_forceLoadScenePath << std::endl;
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
    	else if (IsToken(line, L"ForceLoadProject")) m_editorSettings.m_forceLoadProject = stoi(TokenValue(line));
    	else if (IsToken(line, L"ForceLoadProjectPath")) m_editorSettings.m_forceLoadProjectPath = TokenValue(line);
    	else if (IsToken(line, L"ForceLoadScenePath")) m_editorSettings.m_forceLoadScenePath = TokenValue(line);
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

    	if (ImGui::BeginTabItem("Force Loading"))
    	{
    		ImGui::Checkbox("Force load project and scene on startup", &m_editorSettings.m_forceLoadProject);

			// Use the interface from ProjectHandler
    		std::filesystem::path forceProject;
    		ImGui::Text(WSTR_TO_STR(m_editorSettings.m_forceLoadProjectPath).c_str());
    		ImGui::SameLine();
    		if (ImGui::Button("Select Folder"))
    			if (ProjectHandler::ShowFileSelect(forceProject))
    				m_editorSettings.m_forceLoadProjectPath = forceProject;

    		std::filesystem::path forceScene;
    		ImGui::Text(WSTR_TO_STR(m_editorSettings.m_forceLoadScenePath).c_str());
    		ImGui::SameLine();
    		if (ImGui::Button("Select Scene"))
				if (ProjectHandler::ShowFileSelect(forceScene, SceneFilters))
					m_editorSettings.m_forceLoadScenePath = forceScene;

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
