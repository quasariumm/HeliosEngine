#pragma once

namespace Engine
{

// Forward declare all systems
class EntityComponentSystem;
class Renderer;
class Camera;
namespace Log { class LogHandler; }
namespace Editor { class EditorInterface; }
namespace Physics { class PhysicsCore; }
namespace Audio { class AudioPlayer; }

// =============================================================
// Main system handler
// =============================================================

/// Can be accessed anywhere using Engine::Systems
class SystemsCore
{
public:
    static SystemsCore* Get()
    {
        static SystemsCore instance;
        return &instance;
    }

    SystemsCore() { Initialize(); }
    ~SystemsCore() { Shutdown(); }

    /// Create instance of all systems
    void Initialize();
    /// Delete instances of all systems
    void Shutdown() const;

    // Getters
    [[nodiscard]] Renderer*                 GetRenderer()           const { return m_renderer; }
    [[nodiscard]] Log::LogHandler*          GetLogHandler()         const { return m_logHandler; }
    [[nodiscard]] EntityComponentSystem*    GetECS()                const { return m_entityComponentSystem; }
    [[nodiscard]] Editor::EditorInterface*  GetEditorInterface()    const { return m_editorInterface; }
    [[nodiscard]] Physics::PhysicsCore*     GetPhysics()            const { return m_physics; }
	[[nodiscard]] Audio::AudioPlayer*       GetAudio()              const { return m_audioPlayer; }


private:
    bool m_initialized = false;

    Renderer*                   m_renderer = nullptr;
    Log::LogHandler*            m_logHandler = nullptr;
    EntityComponentSystem*      m_entityComponentSystem = nullptr;
    Editor::EditorInterface*    m_editorInterface = nullptr;
    Physics::PhysicsCore*       m_physics = nullptr;
	Audio::AudioPlayer*			m_audioPlayer = nullptr;

	Camera*						m_camera = nullptr;
};

// Shorthands
namespace Systems
{
    static Renderer*                    GetRenderer()           { return SystemsCore::Get()->GetRenderer(); }
    static Log::LogHandler*             GetLogHandler()         { return SystemsCore::Get()->GetLogHandler(); }
    static EntityComponentSystem*       GetECS()                { return SystemsCore::Get()->GetECS(); }
    static Editor::EditorInterface*     GetEditorInterface()    { return SystemsCore::Get()->GetEditorInterface(); }
    static Physics::PhysicsCore*        GetPhysics()            { return SystemsCore::Get()->GetPhysics(); }
    static Audio::AudioPlayer*			GetAudio()	            { return SystemsCore::Get()->GetAudio(); }
}

}
