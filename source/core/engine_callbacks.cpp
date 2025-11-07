#include "core/engine.hpp"
#include "core/systems.hpp"
#include "rendering/camera.hpp"

using namespace Helios;

void CoreCallbacks::KeyDown( Window& w, Key key )
{
}


void CoreCallbacks::KeyUp( Window& w, Key key )
{

}


void CoreCallbacks::MouseMove( Window& w, glm::vec2 delta )
{
	(void)w;
	Systems::GetCamera()->MouseMove(delta);
}


void CoreCallbacks::MouseDown( Window& w, MouseButton button )
{
	(void)w;
	Systems::GetCamera()->MouseButtonDown(button);
}


void CoreCallbacks::MouseUp( Window& w, MouseButton button )
{
	(void)w;
	Systems::GetCamera()->MouseButtonUp(button);
}


void CoreCallbacks::MouseScroll( Window& w, float x, float y )
{

}


void CoreCallbacks::Resize( Window& w, const glm::uvec2& size )
{

}


void CoreCallbacks::Focus( Window& w, bool focused )
{
	// TODO(Quillan): Add a way for the engine to 'hibernate' when out of focus
}
