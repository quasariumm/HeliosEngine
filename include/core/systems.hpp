#pragma once

namespace Engine
{

// Forward declare all systems
class Renderer;
class Camera;
namespace Log { class LogHandler; }
namespace Physics { class PhysicsCore; }
namespace Audio { class AudioPlayer; }
namespace Editor { class Viewport; }

// =============================================================
// Main system handler
// =============================================================

/// Can be accessed anywhere using Engine::Systems
class SystemsCore
{
public:
    SystemsCore() { Initialize(); }
    ~SystemsCore() { Shutdown(); }

    /// Create instance of all systems
    void Initialize();
    /// Delete instances of all systems
    void Shutdown() const;

    // Getters
    [[nodiscard]] Renderer*                 GetRenderer()           const { return m_renderer; }
    [[nodiscard]] Log::LogHandler*          GetLogHandler()         const { return m_logHandler; }
    [[nodiscard]] Physics::PhysicsCore*     GetPhysics()            const { return m_physics; }
	[[nodiscard]] Audio::AudioPlayer*       GetAudio()              const { return m_audioPlayer; }
	[[nodiscard]] Editor::Viewport*         GetViewport()           const { return m_viewport; }


private:
    bool m_initialized = false;

    Renderer*                   m_renderer = nullptr;
    Log::LogHandler*            m_logHandler = nullptr;
    Physics::PhysicsCore*       m_physics = nullptr;
	Audio::AudioPlayer*			m_audioPlayer = nullptr;
	Editor::Viewport*			m_viewport = nullptr;

	Camera*						m_camera = nullptr;
};

    extern SystemsCore SystemsHandler;

// Shorthands
namespace Systems
{
    static Renderer*                    GetRenderer()           { return SystemsHandler.GetRenderer(); }
    static Log::LogHandler*             GetLogHandler()         { return SystemsHandler.GetLogHandler(); }
    static Physics::PhysicsCore*        GetPhysics()            { return SystemsHandler.GetPhysics(); }
    static Audio::AudioPlayer*			GetAudio()	            { return SystemsHandler.GetAudio(); }
    static Editor::Viewport*			GetViewport()	        { return SystemsHandler.GetViewport(); }
}

}
