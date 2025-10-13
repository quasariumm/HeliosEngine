#include "editor/editor_menus.hpp"

#include "core/engine.hpp"

using namespace Engine;
using namespace Editor;

void EditorMenu::Draw()
{
    if (!ImGui::BeginMainMenuBar()) return;

    if (ImGui::BeginMenu("File"))
    {
        ImGui::EndMenu();
    }

    static bool showStyleEditor = false;
    static bool showMetricsMenu = false;

    if (ImGui::BeginMenu("View"))
    {
        if (ImGui::BeginMenu("Viewport"))
        {
            //ImGui::MenuItem("Statistics", nullptr, &Engine.viewport().drawStatistics);
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Physics"))
        {
            //ImGui::MenuItem("Wireframes", nullptr, &Engine.Physics().drawDebug);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("ImGui"))
        {
            ImGui::MenuItem("Style Editor", nullptr, &showStyleEditor);
            ImGui::MenuItem("Metrics", nullptr, &showMetricsMenu);
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Add"))
    {
        if (ImGui::MenuItem(ICON_CUBE" Empty Object")) CreateSceneObject();
        //if (ImGui::MenuItem(ICON_CUBE_OUTLINE" Debug Cube")) CreateDebugCube();
        ImGui::EndMenu();
    }

    float btn_width = ImGui::GetFontSize() * 3;
    float h_o = btn_width * 0.5f;
    float oh_o = btn_width * 1.5f;

    switch (EngineHandle.GetMode())
    {
    case Mode::Editor:
        ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f - oh_o);
        ImGui::BeginDisabled(); ImGui::Button(ICON_PAUSE, {btn_width, 0}); ImGui::EndDisabled();
        ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f - h_o);
        if (ImGui::Button(ICON_PLAY, {btn_width, 0})) EngineHandle.Play();
        ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f + h_o);
        ImGui::BeginDisabled(); ImGui::Button(ICON_EJECT, {btn_width, 0}); ImGui::EndDisabled();
        break;
    case Mode::Game:
        ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f - oh_o);
        if (ImGui::Button(ICON_PAUSE, {btn_width, 0})) EngineHandle.Pause();
        ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f - h_o);
        if (ImGui::Button(ICON_STOP, {btn_width, 0})) EngineHandle.Stop();
        ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f + h_o);
        if (EngineHandle.GetUsingEditorCam())
        {
            if (ImGui::Button(ICON_CAMERA_LOCK, {btn_width, 0})) EngineHandle.LockCamera();
        }
        else
            if (ImGui::Button(ICON_EJECT, {btn_width, 0})) EngineHandle.ReleaseCamera();
        break;
    case Mode::Paused:
        ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f - oh_o);
        if (ImGui::Button(ICON_PLAY, {btn_width, 0})) EngineHandle.Play();
        ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f - h_o);
        if (ImGui::Button(ICON_STOP, {btn_width, 0})) EngineHandle.Stop();
        ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f + h_o);
        ImGui::BeginDisabled(); ImGui::Button(ICON_EJECT, {btn_width, 0}); ImGui::EndDisabled();
        break;
    }
    
    ImGui::EndMainMenuBar();

    if (showStyleEditor) ImGui::ShowStyleEditor();
    if (showMetricsMenu) ImGui::ShowMetricsWindow();
    
}
