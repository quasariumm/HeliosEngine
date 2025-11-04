#include "core/engine.hpp"

#include <chrono>

#include "audio/audio_player.hpp"
#include "core/ecs.hpp"
#include "editor/editor_menus.hpp"
#include "editor/engine_interface.hpp"
#include "physics/physics.hpp"
#include "rendering/renderer.hpp"
#include "rendering/window.hpp"

using namespace Helios;
using namespace Editor;


namespace Helios
{
Core engineHandle;
}


void Core::Initialize()
{
	systemsHandler.Initialize();
	Systems::GetRenderer()->Initialize();
}


void Core::Shutdown()
{
	systemsHandler.Shutdown();
}


void Core::Run()
{
	auto time = std::chrono::high_resolution_clock::now();

	// TODO(Quillan): Fix main loop
	while (!Systems::GetRenderer()->GetWindow()->ShouldClose())
	{
		auto        ctime   = std::chrono::high_resolution_clock::now();
		auto        elapsed = ctime - time;
		const float dt      = static_cast<float>(static_cast<double>(std::chrono::duration_cast<
			                                    std::chrono::microseconds>(elapsed).count()) /
		                                    1000000.0);

		m_engineStats.SetData(1.0f / dt, dt);

		Systems::GetRenderer()->Prepare();
		EditorInterface::Get()->StartFrame();
		Systems::GetAudio()->Update();

		//m_input->Update();
		//EditorInterface::StartFrame();
		//m_viewport->Prepare();


		ECS::Get()->UpdateEngineSystem(dt);

		if (m_mode == Mode::EDITOR)
			ECS::Get()->UpdateEditorSystem(dt);
		else if (m_mode == Mode::GAME)
			ECS::Get()->UpdateGameSystem(dt);

		DestroyMarkedSceneObjects();

		//if (m_using_editor_cam) m_camera_system->UpdateEditorCamera(dt);

		EditorInterface::Get()->DrawInterfaces();
		Systems::GetViewport()->Draw();
		EditorInterface::Get()->Render();
		Systems::GetRenderer()->Clear();
		EditorInterface::Get()->EndFrame();
		Systems::GetRenderer()->Render();

		time = ctime;

		if (m_fixedStep > 1.0f)
		{
			std::chrono::time_point next = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(
					                               static_cast<long long>(m_fixedStep));
			std::this_thread::sleep_until(next);
		}
	}
}


void Core::Play()
{
	if (m_mode == Mode::GAME)
		return;
	if (m_mode == Mode::EDITOR)
	{
		ECS::Get()->SaveSnapshot();
	}
	m_mode = Mode::GAME;
	LockCamera();
}


void Core::Pause()
{
	if (m_mode == Mode::PAUSED)
		return;
	ReleaseCamera();
	assert(m_mode == Mode::GAME);
	m_mode = Mode::PAUSED;
}


void Core::Stop()
{
	if (m_mode == Mode::EDITOR)
		return;
	ReleaseCamera();
	assert(m_mode == Mode::GAME || m_mode == Mode::PAUSED);
	m_mode = Mode::EDITOR;

	//m_ECS->StopSystems();
	Systems::GetPhysics()->ResetObjects();
	ECS::Get()->LoadSnapshot();
}


void Core::ReleaseCamera()
{
	// if (!m_using_editor_cam)
	//     m_renderer->AssignCamera(m_camera_system->GetEditorCamera());
	m_usingEditorCam = true;
}


void Core::LockCamera()
{
	// if (m_using_editor_cam)
	// {
	//     entt::entity cam = CameraSystem::GetActiveGameCamera();
	//     if (cam != entt::null)
	//         m_renderer->AssignCamera(cam);
	// }
	m_usingEditorCam = false;
}
