#pragma once

namespace Engine
{

struct EditorSettingsData
{
    bool m_closeOnEscape = false;
    glm::ivec2 m_windowSize = {1920, 1080};
    bool m_autoMaximize = true;
    float m_interfaceScaling = 1.0f;
	bool m_forceLoadProject = false;
	std::wstring m_forceLoadProjectPath = L"";
	std::wstring m_forceLoadScenePath = L"";
	glm::uvec2 m_viewportRenderSize = glm::uvec2(1920, 1080);
};

class EditorSettings
{
public:
    static bool Save();
    static bool Load();

    static void OpenWindow() { m_editorSettingsOpen = true; }
    static void CloseWindow() { m_editorSettingsOpen = false; }

    static void DrawWindow();

    static const EditorSettingsData& Get() { return m_editorSettings; };

private:
    static EditorSettingsData m_editorSettings;
    static bool m_editorSettingsOpen;
};

}