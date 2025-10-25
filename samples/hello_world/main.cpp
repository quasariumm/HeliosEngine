#include "audio/audio_player.hpp"
#include "core/engine.hpp"

int main()
{
    Engine::engineHandle.Initialize();

	Engine::Systems::GetAudio()->LoadSound("assets/Testbericht.mp3", false);
	Engine::Systems::GetAudio()->PlaySound("assets/Testbericht.mp3");

    Engine::engineHandle.Run();
    Engine::engineHandle.Shutdown();
    return 0;
}