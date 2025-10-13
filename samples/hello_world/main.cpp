#include "audio/audio_player.hpp"
#include "core/engine.hpp"

int main()
{
    Engine::EngineCore::Get()->Initialize();

	Engine::Systems::GetAudio()->LoadSound("assets/Testbericht.mp3", false);
	Engine::Systems::GetAudio()->PlaySound("assets/Testbericht.mp3");

    Engine::EngineCore::Get()->Run();
    Engine::EngineCore::Get()->Shutdown();
    return 0;
}