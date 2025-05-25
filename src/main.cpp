#include "main.h"
#include <glad/glad.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "Backends/OpenGL46_GLFW/Graphics/GL46_ComputeShader.h"
#include "Backends/OpenGL46_GLFW/Graphics/GL46_Texture2D.h"
#include "Components/Sphere.h"
#include "Components/Material.h"
#include "Core/Window.h"
#include "Debugger/Debugger.h"
#include "Editor/EditorInterface.h"
#include "Editor/EditorSettings.h"
#include "Graphics/Camera.h"
#include "Scene/Scene.h"
#include "Scene/SceneEditor.h"
#include "Viewport/Viewport.h"

#include <tracy/Tracy.hpp>

#ifdef _WIN32
extern "C" {
	__declspec(dllexport) uint32_t NvOptimusEnablement = 0x00000001;
}
#endif

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
	if (severity != GL_DEBUG_SEVERITY_HIGH)
		return;
	std::wcerr << ANSI_ERR <<
		"GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "")
	    << "\n\t\ttype = " << GLDebugTypeToString(type) << ", severity = " << GLSeverityToString(severity)
	    << "\n\t\tmessage = " << message << std::endl;
}

void temp(Engine::Window& window, Engine::Key key)
{
	if (key == Engine::Key::ESCAPE && Engine::EditorSettings::Get().m_closeOnEscape)
		window.SetShouldClose(true);

	if (key == Engine::Key::T)
		Engine::ProjectHandler::m_project->Init();
}

static Engine::Scene g_scene;

extern "C" int __declspec(dllexport) __stdcall main()
{
    std::unique_ptr<Engine::Window> window;
    Engine::CreateWin(
    	window,
    	Engine::vec2u(1280, 720),
    	L"Varför är STL lokaler så irriterande? ありがとうございます",
    	EngineWindowFlags_NoVsync
    );

	window->SetKeyDownCallback(temp);

	glEnable( GL_DEBUG_OUTPUT );
	glDebugMessageCallback( MessageCallback, nullptr );

	// DEbug to get the version and supported extensions
	// const GLubyte* version = glGetString(GL_VERSION);
	// std::string extensions;
	// GLubyte* extension = nullptr;
	// uint32_t i = 0;
	// while (true)
	// {
	// 	extension = const_cast<GLubyte*>(glGetStringi(GL_EXTENSIONS, i));
	// 	try
	// 	{
	// 		extensions += std::string((char*)extension) + "\n";
	// 	} catch (...) {break;}
	// 	++i;
	// }


	Engine::EditorSettings::Load();

	Engine::EditorInterfaceManager::Initialize(window.get());

	Engine::SceneEditor::SetEditingScene(&g_scene);

	Engine::GL46_Texture2D rayTexture;
	rayTexture.FillBlank(window->GetSize().x, window->GetSize().y, 4, Engine::TextureFormat::RGBA32F, true);

	Engine::Viewport::AppendRenderedImage(&rayTexture);

	Engine::DebugWatch(L"pixel0", &rayTexture.GetDataHDR()[0]);
	Engine::DebugWatch(L"pixel1", &rayTexture.GetDataHDR()[1]);
	Engine::DebugWatch(L"pixel2", &rayTexture.GetDataHDR()[2]);

	Engine::GL46_ComputeShader rayCompute;
	rayCompute.LoadFromFile(L"src/Shaders/Raytracing/raytrace.comp");
	const Engine::vec3u computeThreads{
		static_cast<unsigned>(std::ceil(window->GetSize().x / 8.f)),
		static_cast<unsigned>(std::ceil(window->GetSize().y / 8.f)),
		1u
	};

	Engine::ObjectRenderer::SetShader(&rayCompute);

	rayCompute.Use();
	rayCompute.SetInt("OutTexture", 0);
	rayCompute.SetUInt("ScreenWidth", window->GetSize().x);
	rayCompute.SetUInt("ScreenHeight", window->GetSize().y);

    Engine::Timer frameTimer;
	uint32_t frame = 0;
	float deltaTime = 0.f;

	Engine::Camera camera;

	Engine::Viewport::AppendEditorCamera(&camera);

	window->SetMouseButtonDownCallback([&camera](Engine::Window&, Engine::MouseButton button)
	{
		camera.MouseButtonDown(button);
	});

	window->SetMouseButtonUpCallback([&camera](Engine::Window&, Engine::MouseButton button)
	{
		camera.MouseButtonUp(button);
	});

	window->SetMouseMoveCallback([&camera](Engine::Window&, Engine::vec2f diff)
	{
		camera.MouseMove(diff);
	});

    while (!window->ShouldClose())
    {
    	ZoneScopedNC("Frame", tracy::Color::CornflowerBlue);

    	Engine::ObjectRenderer::SendObjectData();

	    {
    		ZoneScopedNC("Input", tracy::Color::LightCoral);
		    window->PollEvents();
			camera.HandleInput(*window, deltaTime);
	    }

	    {
    		ZoneScopedNC("Compute shader dispatch", tracy::Color::LightGreen);
			rayCompute.Use();
			rayCompute.SetUInt("Frame", frame);

			const Engine::vec2u viewportSize(rayTexture.GetWidth(), rayTexture.GetHeight());
			Engine::mat4f camToWorld = camera.GetCamToWorldMatrix(viewportSize);
			rayCompute.SetMat4("CamToWorld", camToWorld);

			Engine::vec3f viewportParams = camera.GetViewportParameters(viewportSize);
			rayCompute.SetVec3("ViewParams", viewportParams);

		    rayTexture.UseCompute(0);
    		rayCompute.SetBool("ClearAccumulator", window->GetKey(Engine::Key::Q) == 1);
    		rayCompute.Dispatch(computeThreads);
			// rayTexture.UpdateData();
	    }

	    {
    		ZoneScopedNC("UI", tracy::Color::LightSteelBlue);
    		{
    			ZoneScopedNC("New Frame", tracy::Color::LightSkyBlue);
    			ImGui_ImplOpenGL3_NewFrame();
    			ImGui_ImplGlfw_NewFrame();
    			ImGui::NewFrame();
    		}

    		Engine::EditorInterfaceManager::SetMouseEnabled(!camera.LockedToViewport());
    		Engine::EditorInterfaceManager::SetKeyboardEnable(!camera.LockedToViewport());

    		Engine::EditorInterfaceManager::Instance().DrawAllInterfaces();

    		{
    			ZoneScopedNC("Render", tracy::Color::LightSkyBlue1);
    			ImGui::Render();
    			window->ClearViewport();

    			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    			ImGui::UpdatePlatformWindows();
    			ImGui::RenderPlatformWindowsDefault();
    		}
	    }

	    {
    		ZoneScopedNC("Frame end", tracy::Color::LightSalmon);
		    window->SwapBuffers();

    		++frame;

    		Engine::DebugWatchTemp<float>(L"DeltaTime", &deltaTime);

    		// Count fps
    		const float frameTime = deltaTime = frameTimer.Elapsed<float>();
    		frameTimer.Reset();

    		static float avg = 10, alpha = 1;
    		avg = (1 - alpha) * avg + alpha * frameTime * 1000;
    		if (alpha > 0.05f) alpha *= 0.5f;
    		const float fps = 1000.0f / avg, rps = (static_cast<float>(rayTexture.GetWidth()) * static_cast<float>(rayTexture.GetHeight())) / avg;

    		if (frame % 50 == 0)
    			window->SetTitle( std::format(L"{0:5.2f}ms ({1:.1f}fps) - {2:.1f}Mrays/s\n", avg, fps, rps / 1000) );
	    }

    	FrameMark;
    }

	Engine::Logger::ExportLog();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    return 0;
}
