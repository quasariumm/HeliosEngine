#include "core/systems.hpp"

#include "debugging/logger.hpp"
#include "core/ecs.hpp"
#include "editor/engine_interface.hpp"
#include "rendering/renderer.hpp"
#include "physics/physics.hpp"

using namespace Engine;

Engine::SystemsCore g_SystemsHandler;

void SystemsCore::Initialize()
{
    if (m_initialized) return;
    m_initialized = true;

    m_renderer = new Renderer();
    m_logHandler = new Log::LogHandler();
    m_entityComponentSystem = new EntityComponentSystem();
    m_editorInterface = new Editor::EditorInterface();
    m_physics = new Physics::PhysicsCore();
}

void SystemsCore::Shutdown() const
{
    delete m_renderer;
    delete m_logHandler;
    delete m_entityComponentSystem;
    delete m_editorInterface;
    delete m_physics;
}