#pragma once
#include "Projects/ProjectHandler.h"

namespace Engine
{
    class EditorInterface;

    static std::vector<EditorInterface*> g_editorInterfaces;

    class EditorInterface
    {
    public:
        explicit EditorInterface(std::string  name) : name(std::move(name)) { g_editorInterfaces.push_back(this); }
        virtual ~EditorInterface() = default;

        virtual void DrawInterface() = 0;

        const std::string name;
        bool active = true;
    };

    class BaseEditorInterface
    {
    public:
        BaseEditorInterface();

        static void DrawAllInterfaces()
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
                    for (EditorInterface* i : g_editorInterfaces)
                        ImGui::MenuItem(i->name.c_str(), nullptr, &i->active);
                    ImGui::MenuItem("ImGui Debugger", nullptr, &showImguiDebug);
                    ImGui::EndMenu();
                }

                ImGui::EndMainMenuBar();
            }

            if (showImguiDebug)
                ImGui::ShowMetricsWindow();

            for (EditorInterface* i : g_editorInterfaces)
                if (i->active)
                i->DrawInterface();
        }
    };
}

