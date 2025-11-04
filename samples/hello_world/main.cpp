#include "audio/audio_player.hpp"
#include "core/engine.hpp"
#include "editor/editor_menus.hpp"
#include "rendering/renderer.hpp"


int main()
{
	Engine::EngineCore::Initialize();

	Engine::Systems::GetAudio()->LoadSound("assets/Testbericht.mp3", false);
	Engine::Systems::GetAudio()->PlaySound("assets/Testbericht.mp3");

	Engine::Systems::GetRenderer()->GetWindow()->SetMouseButtonDownCallback(
			[]( Engine::Window&, const Engine::MouseButton button )
			{
				if (button == Engine::MouseButton::LEFT)
				{
					if (const auto mousePos = Engine::Systems::GetViewport()->GetMousePositionInViewport();
						mousePos.has_value())
					{
						Engine::Editor::SceneGraph::selected = Engine::Systems::GetRenderer()->GetEntityAtCursor(mousePos.value());
					}
				}
			});

	Engine::engineHandle.Run();
	Engine::EngineCore::Shutdown();
	return 0;
}
