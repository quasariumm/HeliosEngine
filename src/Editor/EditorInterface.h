#pragma once

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
            ImGui::DockSpaceOverViewport();

            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Tools"))
                {
                    for (EditorInterface* i : g_editorInterfaces)
                        ImGui::MenuItem(i->name.c_str(), nullptr, &i->active);
                    ImGui::EndMenu();
                }

                ImGui::EndMainMenuBar();
            }

            for (EditorInterface* i : g_editorInterfaces)
                if (i->active)
                i->DrawInterface();
        }
    };
}

