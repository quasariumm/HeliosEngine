#include "core/engine.hpp"

int main()
{
    Engine::EngineCore::Get()->Initialize();
    Engine::EngineCore::Get()->Run();
    Engine::EngineCore::Get()->Shutdown();
    return 0;
}