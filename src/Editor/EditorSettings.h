#pragma once

namespace Engine
{

struct EditorSettingsData
{
    bool m_closeOnEscape = false;
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