#include "EditorInterface.h"

#include "Projects/ProjectHandler.h"

namespace Engine
{

void EditorInterfaceManager::Initialize()
{
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable | ImGuiConfigFlags_NavEnableKeyboard;

    io.Fonts->AddFontFromFileTTF("extern/imgui/misc/fonts/Roboto-Medium.ttf", 16.0f);

    ImFontConfig config;
    config.MergeMode = true;
    config.GlyphMinAdvanceX = 16.0f;
    static constexpr ImWchar32 icon_ranges[] = {ICON_MIN_MDI, ICON_MAX_MDI, 0};
    io.Fonts->AddFontFromFileTTF("extern/fonts/materialdesignicons-webfont.ttf", 16.0f, &config, icon_ranges);
}

void EditorInterfaceManager::DrawAllInterfaces() const
{
    ProjectHandler::ProjectWindows();

    ImGui::DockSpaceOverViewport();

    static bool showImguiDebug = false;

    if (ImGui::BeginMainMenuBar())
    {
        std::string projectName;
        if (ProjectLoaded())
            projectName = std::string(ICON_CONTROLLER" ") + ProjectName();
        else
            projectName =  ICON_ALERT" No project loaded";

        if (ImGui::BeginMenu(projectName.c_str()))
        {
            if (ImGui::MenuItem(ICON_UPLOAD_CIRCLE" Open project"))
                ProjectHandler::ShowProjectSelector(true);
            if (ImGui::MenuItem(ICON_DOWNLOAD_CIRCLE" New project"))
                ProjectHandler::ShowProjectCreator(true);

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu(ICON_TOOLS" Tools"))
        {
            for (const auto& i : m_editorInterfaces)
                ImGui::MenuItem(i.second->name.c_str(), nullptr, &i.second->active);
            ImGui::MenuItem("ImGui Debugger", nullptr, &showImguiDebug);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    if (showImguiDebug)
        ImGui::ShowMetricsWindow();

    for (const auto& i : m_editorInterfaces)
        if (i.second->active)
            i.second->DrawInterface();
}
}
