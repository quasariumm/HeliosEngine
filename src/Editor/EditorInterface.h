#pragma once
#include <vector>

#include "imgui.h"
#include "Core/Window.h"


namespace Engine
{
    class EditorInterface;

    static std::vector<EditorInterface*> g_editorInterfaces;

    class EditorInterface
    {
    public:
        EditorInterface() { g_editorInterfaces.push_back(this); }
        virtual ~EditorInterface() = default;

        virtual void DrawInterface() = 0;
    };


    class BaseEditorInterface
    {
    public:
        BaseEditorInterface();

        static void DrawAllInterfaces()
        {
            ImGui::DockSpaceOverViewport();

            for (EditorInterface* i : g_editorInterfaces)
                i->DrawInterface();
        }
    };
}

