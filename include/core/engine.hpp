#pragma once

#include "statistics.hpp"
#include "rendering/window.hpp"
#include <memory>

#include "editor/editor_menus.hpp"

// The main loop for the engine
// Controls only the timestep, runtime state, and camera state

namespace Engine
{

enum class Mode
{
    Editor,  // Editor mode, editor time
    Game,    // Game mode, run time
    Paused,  // Game is paused
};

class EngineCore
{
public:
    void Initialize();
    void Shutdown();
    void Run();

    void Play();
    void Pause();
    void Stop();

    void ReleaseCamera();
    void LockCamera();

    void SetFixedTimeStep(const float& ms) { m_fixed_step = ms; }

    [[nodiscard]] Mode GetMode() const { return m_mode; }
    [[nodiscard]] bool GetUsingEditorCam() const { return m_using_editor_cam; }
    [[nodiscard]] EngineStats GetEngineStats() const { return m_engine_stats; }

private:
    bool m_using_editor_cam = true;
    Mode m_mode = Mode::Editor;
    EngineStats m_engine_stats = {};
    float m_fixed_step = -1.0f;

    std::unique_ptr<Window> window;
};

extern EngineCore EngineHandle;

}
