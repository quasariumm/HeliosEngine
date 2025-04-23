#include <glad/glad.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "Backends/OpenGL46_GLFW/Graphics/GL46_ComputeShader.h"
#include "Backends/OpenGL46_GLFW/Graphics/GL46_Texture2D.h"
#include "Components/Sphere.h"
#include "Core/Window.h"
#include "Debugger/Debugger.h"
#include "Editor/EditorInterface.h"
#include "Editor/EditorSettings.h"
#include "Graphics/Camera.h"
#include "Scene/Scene.h"
#include "Scene/SceneEditor.h"
#include "Viewport/Viewport.h"

#include <tracy/Tracy.hpp>

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

	if (key == Engine::Key::TILDE)
		window.SetShouldClose(true);
}

static Engine::Scene g_scene;

int main(int, char**)
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

	Engine::SceneObject* testObject = g_scene.NewObject();
	testObject->GetTransform()->position() = {3,3,3};
	DebugWatch(L"Test Object", testObject->GetTransform()->positionRef());
    auto* sphere = testObject->AddComponent<Engine::Sphere>();
    Engine::DebugWatch(L"Radius", &sphere->m_radius);

	// Add a sphere to the scene
	rayCompute.SetInt("NumSpheres", 2);
	const std::string baseName = "Spheres[0]";
	rayCompute.SetVec3(baseName + ".center", testObject->GetTransform()->position());
	rayCompute.SetFloat(baseName + ".radius", 1.f);

	const std::string matBaseName = baseName + ".material";
	rayCompute.SetVec3(matBaseName + ".diffuseColor", Engine::vec3f(0.9f, 0.6f, 0.3f));
	rayCompute.SetVec3(matBaseName + ".specularColor", Engine::vec3f(1.f));
	rayCompute.SetFloat(matBaseName + ".shininess", 0.f);
	rayCompute.SetFloat(matBaseName + ".specularProbability", 1.f);

	rayCompute.SetVec3(matBaseName + ".emissionColor", Engine::vec3f(1.f));
	rayCompute.SetFloat(matBaseName + ".emissionStrength", 0.f);

	rayCompute.SetFloat(matBaseName + ".refractionProbability", 0.f);
	rayCompute.SetFloat(matBaseName + ".refractionCoefficient", 1.f);

	const std::string baseName2 = "Spheres[1]";
	rayCompute.SetVec3(baseName2 + ".center", Engine::vec3f(0.f, -32.f, -6.f));
	rayCompute.SetFloat(baseName2 + ".radius", 30.f);

	const std::string matBaseName2 = baseName2 + ".material";
	rayCompute.SetVec3(matBaseName2 + ".diffuseColor", Engine::vec3f(1.f));
	rayCompute.SetVec3(matBaseName2 + ".specularColor", Engine::vec3f(1.f));
	rayCompute.SetFloat(matBaseName2 + ".shininess", 0.f);
	rayCompute.SetFloat(matBaseName2 + ".specularProbability", 1.f);

	rayCompute.SetVec3(matBaseName2 + ".emissionColor", Engine::vec3f(1.f));
	rayCompute.SetFloat(matBaseName2 + ".emissionStrength", 0.f);

	rayCompute.SetFloat(matBaseName2 + ".refractionProbability", 0.f);
	rayCompute.SetFloat(matBaseName2 + ".refractionCoefficient", 1.f);

    Engine::Timer frameTimer;
	uint64_t frame = 0;
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
    	ZoneScopedN("Frame");
        window->PollEvents();

    	camera.HandleInput(*window, deltaTime);

    	rayCompute.Use();

    	rayCompute.SetUInt("Frame", frame % 1000);

    	const std::string baseName = "Spheres[0]";
    	rayCompute.SetVec3(baseName + ".center", testObject->GetTransform()->position());
    	rayCompute.SetFloat(baseName + ".radius", sphere->m_radius);

    	const Engine::vec2u viewportSize(rayTexture.GetWidth(), rayTexture.GetHeight());
    	Engine::mat4f camToWorld = camera.GetCamToWorldMatrix(viewportSize);
    	rayCompute.SetMat4("CamToWorld", camToWorld);

    	Engine::vec3f viewportParams = camera.GetViewportParameters(viewportSize);
    	rayCompute.SetVec3("ViewParams", viewportParams);

	    {
    		ZoneScopedN("Compute shader dispatch");
		    rayTexture.UseCompute(0);
    		rayCompute.Dispatch(computeThreads);
			// rayTexture.UpdateData();
	    }

	    {
    		ZoneScopedN("UI");
		    ImGui_ImplOpenGL3_NewFrame();
    		ImGui_ImplGlfw_NewFrame();
    		ImGui::NewFrame();

    		Engine::EditorInterfaceManager::SetMouseEnabled(!camera.LockedToViewport());
    		Engine::EditorInterfaceManager::SetKeyboardEnable(!camera.LockedToViewport());

    		Engine::EditorInterfaceManager::Instance().DrawAllInterfaces();

    		ImGui::Render();
    		window->ClearViewport();

    		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    		ImGui::UpdatePlatformWindows();
    		ImGui::RenderPlatformWindowsDefault();
	    }

        window->SwapBuffers();

    	++frame;
    	FrameMark;

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

	Engine::Logger::ExportLog();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    return 0;
}
