#include "rendering/window.hpp"

#include "../backends/opengl46_glfw/core/gl46_window.hpp"


namespace Helios
{
const std::string& Window::GetTitle() const
{
	return m_title;
}


void Window::RequestClose()
{
	m_shouldClose = true;
}


void Window::SetResizeCallback( const resizeCallback_t& callback )
{
	onResize = callback;
}


void Window::SetFocusCallback( const focusCallback_t& callback )
{
	onFocus = callback;
}


void Window::SetMouseMoveCallback( const mouseMoveCallback_t& callback )
{
	onMouseMove = callback;
}


void Window::SetMouseButtonDownCallback( const mouseButtonCallback_t& callback )
{
	onMouseDown = callback;
}


void Window::SetMouseButtonUpCallback( const mouseButtonCallback_t& callback )
{
	onMouseUp = callback;
}


void Window::SetMouseScrollCallback( const mouseScrollCallback_t& callback )
{
	onMouseScroll = callback;
}


void Window::SetKeyDownCallback( const keyCallback_t& callback )
{
	onKeyDown = callback;
}


void Window::SetKeyUpCallback( const keyCallback_t& callback )
{
	onKeyUp = callback;
}


void CreateWin( std::unique_ptr<Window>& window,
                const glm::uvec2&        size,
                const std::string&       title,
                const uint32_t           flags )
{
#if defined HELIOS_API_VK
#elif defined HELIOS_API_DX12
#elif defined HELIOS_API_GL46
	if (auto GLwindow = std::make_unique<GL46_Window>();
		GLwindow->Init(size, title, flags))
		window = std::move(GLwindow);
#else
	throw std::runtime_error("Please select something. This is not yet supported")
#endif
}
} // Engine
