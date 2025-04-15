#include <iostream>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <glad/glad.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "Backends/OpenGL46_GLFW/Graphics/GL46_ComputeShader.h"
#include "Backends/OpenGL46_GLFW/Graphics/GL46_Texture2D.h"
#include "Core/Common.h"
#include "Core/Timer.h"
#include "Core/Window.h"
#include "Editor/EditorInterface.h"
#include "Scene/Scene.h"
#include "Scene/SceneEditor.h"

void GLAPIENTRY MessageCallback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam
	)
{
	if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
		return;
	std::wcerr << ANSI_ERR <<
		"GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "")
	    << "\n\t\ttype = " << GLDebugTypeToString(type) << ", severity = " << GLSeverityToString(severity)
	    << "\n\t\tmessage = " << message << std::endl;
}

void temp(Engine::Window& window, Engine::Key key)
{
	if (key == Engine::Key::ESCAPE)
		window.SetShouldClose(true);
}

static Engine::Scene g_scene;

int main(int, char**)
{
    std::unique_ptr<Engine::Window> window;
    Engine::CreateWin(
    	window,
    	Engine::vec2u(600, 400),
    	L"Varför är STL lokaler så irriterande?",
    	EngineWindowFlags_NoVsync
    );

	window->SetKeyDownCallback(temp);

	glEnable( GL_DEBUG_OUTPUT );
	glDebugMessageCallback( MessageCallback, nullptr );

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
	rayCompute.SetUInt("ScreenWidth", window->GetSize().x);
	rayCompute.SetUInt("ScreenHeight", window->GetSize().y);

    Engine::Timer frameTimer;
	uint64_t frame = 0;
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

    	++frame;

    	// Count fps
    	const float frameTime = frameTimer.Elapsed<float>();
    	frameTimer.Reset();

    	static float avg = 10, alpha = 1;
    	avg = (1 - alpha) * avg + alpha * frameTime * 1000;
    	if (alpha > 0.05f) alpha *= 0.5f;
    	const float fps = 1000.0f / avg, rps = (static_cast<float>(window->GetSize().x) * static_cast<float>(window->GetSize().y)) / avg;

    	if (frame % 200 == 0)
			window->SetTitle( std::format(L"{0:5.2f}ms ({1:.1f}fps) - {2:.1f}Mrays/s\n", avg, fps, rps / 1000) );
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    return 0;
}
