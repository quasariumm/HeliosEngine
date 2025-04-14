#include <imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "Backends/OpenGL46_GLFW/Core/GL46_Window.h"
#include "Scene/Scene.h"
#include "Scene/SceneEditor.h"

Engine::Scene g_scene;
Engine::SceneEditor g_sceneEditor(&g_scene);

int main(int argc, char* argv[])
{
    Engine::GL46_Window window{};
    window.Init({600, 400}, L"Varför är STL lokaler så irriterande?");

    while (!window.ShouldClose())
    {

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        g_sceneEditor.SceneTreeEditor();
        g_sceneEditor.ObjectEditor();


        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window.PollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();


    return 0;
}
