#include "core/systems.hpp"

#include "debugging/logger.hpp"
#include "rendering/renderer.hpp"
#include "physics/physics.hpp"
#include "audio/audio_player.hpp"
#include "editor/editor_menus.hpp"
#include "rendering/camera.hpp"

using namespace Engine;

namespace Engine {
	SystemsCore SystemsHandler;
}

void SystemsCore::Initialize()
{
    if (m_initialized) return;
    m_initialized = true;

    m_renderer = new Renderer();
    m_logHandler = new Log::LogHandler();
    m_physics = new Physics::PhysicsCore();
	m_camera = new Camera();
	m_audioPlayer = new Audio::AudioPlayer(m_camera);
	m_viewport = new Editor::Viewport();
}

void SystemsCore::Shutdown() const
{
    delete m_renderer;
    delete m_logHandler;
    delete m_physics;
	delete m_camera;
	delete m_audioPlayer;
	delete m_viewport;
}