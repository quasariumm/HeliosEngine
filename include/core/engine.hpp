#pragma once

#include <memory>
#include "statistics.hpp"
#include "rendering/window.hpp"

// The main loop for the engine
// Controls only the timestep, runtime state, and camera state

namespace Engine
{
enum class Mode
{
	EDITOR,
	// Editor mode, editor time
	GAME,
	// Game mode, run time
	PAUSED,
	// Game is paused
};


class EngineCore
{
public:

	static void Initialize();

	static void Shutdown();

	void Run();

	void Play();

	void Pause();

	void Stop();

	void ReleaseCamera();

	void LockCamera();

	void SetFixedTimeStep( const float& ms ) { m_fixedStep = ms; }

	[[nodiscard]] Mode        GetMode() const { return m_mode; }
	[[nodiscard]] bool        GetUsingEditorCam() const { return m_usingEditorCam; }
	[[nodiscard]] EngineStats GetEngineStats() const { return m_engineStats; }

private:

	bool        m_usingEditorCam = true;
	Mode        m_mode           = Mode::EDITOR;
	EngineStats m_engineStats    = {};
	float       m_fixedStep      = -1.0f;

	std::unique_ptr<Window> m_window;
};


extern EngineCore engineHandle;
}
