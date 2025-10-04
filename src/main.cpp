#include "main.h"
#include <glad/glad.h>

#include "Backends/OpenGL46_GLFW/Graphics/GL46_ComputeShader.h"
#include "Backends/OpenGL46_GLFW/Graphics/GL46_Texture2D.h"
#include "Components/Sphere.h"
#include "Components/Material.h"
#include "Components/Light.h"
#include "Components/Model.h"
#include "Components/Skybox.h"
#include "Core/Window.h"
#include "Debugger/Debugger.h"
#include "Editor/EditorInterface.h"
#include "Editor/EditorSettings.h"
#include "Graphics/Camera.h"
#include "Scene/Scene.h"
#include "Scene/SceneEditor.h"
#include "Viewport/Viewport.h"

#include <tracy/Tracy.hpp>

#include "Audio/AudioPlayer.h"
#include "Scene/SceneStorage.h"

#ifdef _WIN32
extern "C" {
	__declspec(dllexport) uint32_t NvOptimusEnablement = 0x00000001;
}
#endif


static bool hasWarnedOfBrokenShader = false;
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

	if (strcmp(message, "GL_INVALID_OPERATION error generated. <program> has not been linked, or is not a program object.") == 0
		|| strcmp(message, "GL_INVALID_OPERATION error generated. <program> object is not successfully linked, or is not a program object.") == 0
		|| strcmp(message, "GL_INVALID_OPERATION error generated. No active program.") == 0
		|| strcmp(message, "GL_INVALID_OPERATION error generated. No active compute shader.") == 0
		|| strcmp(message, "GL_INVALID_VALUE error generated. Handle does not refer to a shader or program object.") == 0)
	{
		if (!hasWarnedOfBrokenShader)
		{
			hasWarnedOfBrokenShader = true;
			std::wcerr << ANSI_ERR << "Shader is broken!!!" << std::endl;
		}
		return;
	}

	std::wcerr << ANSI_ERR <<
		"GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "")
	    << "\n\t\ttype = " << GLDebugTypeToString(type) << ", severity = " << GLSeverityToString(severity)
	    << "\n\t\tmessage = " << message << std::endl;
}

void temp(Engine::Window& window, Engine::Key key)
{
	if (key == Engine::Key::ESCAPE && Engine::EditorSettings::Get().m_closeOnEscape)
		window.RequestClose();

	if (key == Engine::Key::B)
	{
		int compCount = Engine::ComponentRegister::Instance().m_registry.size();
		Engine::DebugLog( Engine::LogSeverity::INFO,  STR_TO_WSTR(std::to_string(compCount)));
	}

	if (key == Engine::Key::T && Engine::ProjectHandler::ProjectLoaded())
		Engine::ProjectHandler::m_project->Init();
}

static Engine::Scene g_scene;

extern "C" int __declspec(dllexport) __stdcall main()
{
#ifdef ENGINE_BUILD_DLL
	std::cout << "Running engine library as engine" << std::endl;
#else
	std::cout << "Running engine library as include" << std::endl;
#endif

	Engine::EditorSettings::Load();

    std::unique_ptr<Engine::Window> window;
    Engine::CreateWin(
    	window,
    	glm::uvec2(Engine::EditorSettings::Get().m_windowSize.x, Engine::EditorSettings::Get().m_windowSize.y),
    	L"Helios Engine",
    	EngineWindowFlags_NoVsync
    );

	if (Engine::EditorSettings::Get().m_autoMaximize)
		window->SetMaximized(true);

	window->SetKeyDownCallback(temp);

	glEnable( GL_DEBUG_OUTPUT );
	glDebugMessageCallback( MessageCallback, nullptr );

	Engine::EditorInterfaceManager::Initialize(window.get());

	Engine::SceneEditor::SetEditingScene(&g_scene);

	Engine::GL46_Texture2D rayTexture;
	// Get the viewport render size from the Editor Settings
	glm::uvec2 viewportSize = Engine::EditorSettings::Get().m_viewportRenderSize;
	rayTexture.FillBlank(viewportSize.x, viewportSize.y, 4, Engine::TextureFormat::RGBA32F, true);

	Engine::Viewport::AppendRenderedImage(&rayTexture);

	Engine::DebugWatch(L"pixel0", &rayTexture.GetDataHDR()[0]);
	Engine::DebugWatch(L"pixel1", &rayTexture.GetDataHDR()[1]);
	Engine::DebugWatch(L"pixel2", &rayTexture.GetDataHDR()[2]);

	Engine::GL46_ComputeShader rayCompute;
	rayCompute.LoadFromFile(L"src/Shaders/Raytracing/raytrace.comp");
	const glm::uvec3 computeThreads{
		static_cast<unsigned>(std::ceil(rayTexture.GetWidth() / 8.f)),
		static_cast<unsigned>(std::ceil(rayTexture.GetHeight() / 8.f)),
		1u
	};

	Engine::ObjectRenderer::Instance().SetShader(&rayCompute);

	rayCompute.Use();
	rayCompute.SetInt("OutTexture", 0);
	rayCompute.SetUInt("ScreenWidth", rayTexture.GetWidth());
	rayCompute.SetUInt("ScreenHeight", rayTexture.GetHeight());
	
    Engine::Timer frameTimer;
	uint32_t frame = 0;
	float deltaTime = 0.f;

	Engine::Camera camera;
	glm::mat4 VPMat;
	glm::mat4 prevVPMat;

	Engine::Viewport::AppendEditorCamera(&camera);

#if !(defined __MINGW64__ || defined __MINGW32__)
	// Audio test
	Engine::Audio::AudioPlayer audioPlayer{&camera};
	audioPlayer.LoadSound("assets/Testbericht.mp3", false);
	audioPlayer.PlaySound("assets/Testbericht.mp3");
#endif

	// Setting default material
	auto defaultMaterial = Engine::Material();
	defaultMaterial.m_properties = { 1, 0, 1, 1, 1, 1 };
	defaultMaterial.m_microfacetModel = { 1, 0, 0, 0 };
	defaultMaterial.m_diffuseColor = glm::vec3(0.9f, 0.6f, 0.3f);
	defaultMaterial.m_specularColor = glm::vec3(1.f, 1.f, 1.f);
	defaultMaterial.m_refractionCoefficient = 1.f;
	Engine::MaterialRegister::Instance().SetDefaultMaterial(&defaultMaterial);
	Engine::ObjectRenderer::Instance().UpdateMaterialSSBO();

	window->SetMouseButtonDownCallback([&camera](Engine::Window&, Engine::MouseButton button)
	{
		camera.MouseButtonDown(button);
	});

	window->SetMouseButtonUpCallback([&camera](Engine::Window&, Engine::MouseButton button)
	{
		camera.MouseButtonUp(button);
	});

	window->SetMouseMoveCallback([&camera](Engine::Window&, glm::vec2 diff)
	{
		camera.MouseMove(diff);
	});

	// Force load project if set
	const Engine::EditorSettingsData& editorSettings = Engine::EditorSettings::Get();
	if (editorSettings.m_forceLoadProject)
	{
		std::filesystem::path projectPath(editorSettings.m_forceLoadProjectPath);
		Engine::ProjectHandler::LoadProject(projectPath);
		std::filesystem::path sceneFile(editorSettings.m_forceLoadScenePath);
		Engine::SceneLoader::LoadFromFile(Engine::SceneEditor::m_targetScene, sceneFile);
		Engine::SceneEditor::m_sceneFile = sceneFile;
	}

	while (!window->ShouldClose())
    {
#if !(defined __MINGW64__ || defined __MINGW32__)
		audioPlayer.Update();
#endif
    	ZoneScopedNC("Frame", tracy::Color::CornflowerBlue);

    	Engine::ObjectRenderer::Instance().SendObjectData();

	    {
    		ZoneScopedNC("Input", tracy::Color::LightCoral);
		    window->PollEvents();
			camera.HandleInput(*window, deltaTime);
	    }

    	{
    		ZoneScopedNC("Updating scene", tracy::Color::DarkBlue);
    		if (Engine::SceneEditor::m_targetScene != nullptr && !Engine::ProjectHandler::m_lockOut)
    			Engine::SceneEditor::m_targetScene->TickObjects();
    	}

	    {
    		
    		ZoneScopedNC("Compute shader dispatch", tracy::Color::LightGreen);
			rayCompute.Use();
			rayCompute.SetUInt("Frame", frame);

			const glm::uvec2 viewportSize(rayTexture.GetWidth(), rayTexture.GetHeight());
			glm::mat4 camToWorld = camera.GetCamToWorldMatrix();
			rayCompute.SetMat4("CamToWorld", camToWorld);

    		prevVPMat = VPMat;
    		VPMat = camera.GetProjectionMatrix(viewportSize) * camera.GetViewMatrix();
    		
    		rayCompute.SetMat4("VPMat", VPMat);

    		rayCompute.SetMat4("PrevVPMat", prevVPMat);
    		
			glm::vec3 viewportParams = camera.GetViewportParameters(viewportSize);
			rayCompute.SetVec3("ViewParams", viewportParams);

		    rayTexture.UseCompute(0);
    		if (window->GetKey(Engine::Key::Q))
    			frame = 0;
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
    			window->SetTitle( std::format(L"Helios Engine | {0} | {1:5.2f}ms ({2:.1f}fps) - {3:.1f}Mrays/s\n", Engine::ProjectHandler::ProjectName(), avg, fps, rps / 1000) );
	    }

    	FrameMark;
    }

	Engine::Logger::ExportLog();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    return 0;
}
