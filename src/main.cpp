#include <imgui.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "Core/Window.h"
#include "Editor/EditorInterface.h"
#include "Scene/Scene.h"
#include "Scene/SceneEditor.h"

static Engine::Scene g_scene;

int main(int argc, char* argv[])
{
    std::unique_ptr<Engine::Window> window;
    Engine::CreateWin(window, Engine::vec2u(600, 400), L"Varför är STL lokaler så irriterande?");

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable | ImGuiConfigFlags_NavEnableKeyboard;

    Engine::SceneEditor editor = Engine::SceneEditor(&g_scene);

    while (!window->ShouldClose())
    {
        window->PollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        Engine::BaseEditorInterface::DrawAllInterfaces();

        ImGui::Render();
        window->ClearViewport();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();

        window->SwapBuffers();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    return 0;
}
