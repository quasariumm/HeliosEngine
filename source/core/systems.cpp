#include "core/systems.hpp"

#include "audio/audio_player.hpp"
#include "debugging/logger.hpp"
#include "editor/editor_menus.hpp"
#include "physics/physics.hpp"
#include "rendering/camera.hpp"
#include "rendering/renderer.hpp"

using namespace Engine;


namespace Engine
{
SystemsCore systemsHandler;
}


void SystemsCore::Initialize()
{
	if (m_initialized)
		return;
	m_initialized = true;

	m_renderer    = new Renderer();
	m_logHandler  = new Log::LogHandler();
	m_physics     = new Physics::PhysicsCore();
	m_camera      = new Camera();
	m_audioPlayer = new Audio::AudioPlayer(m_camera);
	m_viewport    = new Editor::Viewport();
}


void SystemsCore::Shutdown() const
{
	if (m_renderer)		delete m_renderer;
	if (m_logHandler)	delete m_logHandler;
	if (m_physics)		delete m_physics;
	if (m_camera)		delete m_camera;
	if (m_audioPlayer)	delete m_audioPlayer;
	if (m_viewport)		delete m_viewport;
}
