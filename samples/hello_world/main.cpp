#include "audio/audio_player.hpp"
#include "core/engine.hpp"

int main()
{
    Engine::EngineHandle.Initialize();

	Engine::Systems::GetAudio()->LoadSound("assets/Testbericht.mp3", false);
	Engine::Systems::GetAudio()->PlaySound("assets/Testbericht.mp3");

    Engine::EngineHandle.Run();
    Engine::EngineHandle.Shutdown();
    return 0;
}