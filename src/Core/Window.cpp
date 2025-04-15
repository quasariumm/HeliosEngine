#include "Window.h"

#include <memory>

#include "Backends/OpenGL46_GLFW/Core/GL46_Window.h"

namespace Engine
{

void Window::SetResizeCallback(const resizeCallback_t& callback)
{
	onResize = callback;
}


void Window::SetFocusCallback(const focusCallback_t& callback)
{
	onFocus = callback;
}


void Window::SetMouseMoveCallback(const mouseMoveCallback_t& callback)
{
	onMouseMove = callback;
}


void Window::SetMouseButtonDownCallback(const mouseButtonCallback_t& callback)
{
	onMouseDown = callback;
}


void Window::SetMouseButtonUpCallback(const mouseButtonCallback_t& callback)
{
	onMouseUp = callback;
}


void Window::SetMouseScrollCallback(const mouseScrollCallback_t& callback)
{
	onMouseScroll = callback;
}


void Window::SetKeyDownCallback(const keyCallback_t& callback)
{
	onKeyDown = callback;
}


void Window::SetKeyUpCallback(const keyCallback_t& callback)
{
	onKeyUp = callback;
}


void CreateWin(std::unique_ptr<Window>& window, const vec2u& size, const std::wstring& title, const uint64_t flags, const GraphicsAPI api)
{
	switch (api)
	{
	case GraphicsAPI::VULKAN:
		break;
	case GraphicsAPI::DIRECTX12:
		break;
	case GraphicsAPI::OPENGL:
	{
		std::unique_ptr<Engine::GL46_Window> GLwindow = std::make_unique<Engine::GL46_Window>();
		if( GLwindow->Init( size, title, flags ) )
			window = std::move(GLwindow);
		break;
	}
	case GraphicsAPI::NONE:
	default:
	{
		std::unique_ptr<Engine::GL46_Window> GLwindow = std::make_unique<Engine::GL46_Window>();
		if( GLwindow->Init( size, title, flags ) )
			window = std::move(GLwindow);
	}
	}
}

} // Engine