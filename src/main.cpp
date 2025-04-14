#include <imgui.h>
#include <iostream>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "Backends/OpenGL46_GLFW/Graphics/GL46_ComputeShader.h"
#include "Backends/OpenGL46_GLFW/Graphics/GL46_Texture2D.h"
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

	Engine::GL46_Texture2D rayTexture;
	rayTexture.FillBlank(window->GetSize().x, window->GetSize().y, 4, Engine::TextureFormat::RGBA32F, true);

	Engine::GL46_ComputeShader rayCompute;
	rayCompute.LoadFromFile(L"src/Shaders/raytrace.comp");
	const Engine::vec3u computeThreads{
		static_cast<unsigned>(std::ceil(window->GetSize().x / 8.f)),
		static_cast<unsigned>(std::ceil(window->GetSize().y / 8.f)),
		1u
	};

	rayCompute.Use();
	rayCompute.SetInt("OutTexture", 0);

    while (!window->ShouldClose())
    {
        window->PollEvents();
    	rayCompute.Use();
    	rayTexture.UseCompute(0);
    	rayCompute.Dispatch(computeThreads);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        Engine::BaseEditorInterface::DrawAllInterfaces();

        ImGui::Begin("Texture");

    	ImGui::Image((ImTextureID)(intptr_t)rayTexture.GetID(), ImVec2(static_cast<float>(rayTexture.GetWidth()), static_cast<float>(rayTexture.GetHeight())));

    	ImGui::End();

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
