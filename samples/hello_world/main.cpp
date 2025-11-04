#include "audio/audio_player.hpp"
#include "core/engine.hpp"
#include "editor/editor_menus.hpp"
#include "rendering/renderer.hpp"


int main()
{
	Helios::Core::Initialize();

	Helios::Systems::GetAudio()->LoadSound("assets/Testbericht.mp3", false);
	Helios::Systems::GetAudio()->PlaySound("assets/Testbericht.mp3");

	Helios::Systems::GetRenderer()->GetWindow()->SetMouseButtonDownCallback(
			[]( Helios::Window&, const Helios::MouseButton button )
			{
				if (button == Helios::MouseButton::LEFT)
				{
					if (const auto mousePos = Helios::Systems::GetViewport()->GetMousePositionInViewport();
						mousePos.has_value())
					{
						Helios::Editor::SceneGraph::selected = Helios::Systems::GetRenderer()->GetEntityAtCursor(mousePos.value());
					}
				}
			});

	Helios::engineHandle.Run();
	Helios::Core::Shutdown();
	return 0;
}
