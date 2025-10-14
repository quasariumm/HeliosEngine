#include "core/engine.hpp"

#include <chrono>

#include "audio/audio_player.hpp"
#include "core/ecs.hpp"
#include "editor/engine_interface.hpp"
#include "rendering/renderer.hpp"
#include "rendering/window.hpp"
#include "physics/physics.hpp"

using namespace Engine;
using namespace Editor;

namespace Engine {
    EngineCore EngineHandle;
}

void EngineCore::Initialize()
{
    SystemsHandler.Initialize();
	Compute::Program::Initialize();
    Systems::GetRenderer()->Initialize();
}

void EngineCore::Shutdown()
{
    SystemsHandler.Shutdown();
}

void EngineCore::Run()
{
    auto time = std::chrono::high_resolution_clock::now();

    // TODO(Quillan): Fix main loop
    while (!Systems::GetRenderer()->GetWindow()->ShouldClose())
    {
        auto ctime = std::chrono::high_resolution_clock::now();
        auto elapsed = ctime - time;
        float dt = (float)((double)std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / 1000000.0);

        m_engine_stats.SetData(1.0f / dt, dt);

        Systems::GetRenderer()->Prepare();
        Editor::EditorInterface::Get()->StartFrame();
        Systems::GetAudio()->Update();

        //m_input->Update();
        //EditorInterface::StartFrame();
        //m_viewport->Prepare();


        ECS::Get()->UpdateEngineSystem(dt);

        if (m_mode == Mode::Editor)
            ECS::Get()->UpdateEditorSystem(dt);
        else if (m_mode == Mode::Game)
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

        if (m_fixed_step > 1.0f)
        {
            std::chrono::time_point next = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds((long long)m_fixed_step);
            std::this_thread::sleep_until(next);
        }
    }
}

void EngineCore::Play()
{    
    if (m_mode == Mode::Game) return;
    if (m_mode == Mode::Editor)
    {
        ECS::Get()->SaveSnapshot();
    }
    m_mode = Mode::Game;
    LockCamera();
}

void EngineCore::Pause()
{
    if (m_mode == Mode::Paused) return;
    ReleaseCamera();
    assert(m_mode == Mode::Game);
    m_mode = Mode::Paused;
}

void EngineCore::Stop()
{
    if (m_mode == Mode::Editor) return;
    ReleaseCamera();
    assert(m_mode == Mode::Game || m_mode == Mode::Paused);
    m_mode = Mode::Editor;
    
    //m_ECS->StopSystems();
    Systems::GetPhysics()->ResetObjects();
    ECS::Get()->LoadSnapshot();
}

void EngineCore::ReleaseCamera()
{
    // if (!m_using_editor_cam)
    //     m_renderer->AssignCamera(m_camera_system->GetEditorCamera());
    m_using_editor_cam = true;
}

void EngineCore::LockCamera()
{
    // if (m_using_editor_cam)
    // {
    //     entt::entity cam = CameraSystem::GetActiveGameCamera();
    //     if (cam != entt::null)
    //         m_renderer->AssignCamera(cam);
    // }
    m_using_editor_cam = false;
}
