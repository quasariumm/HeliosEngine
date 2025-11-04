#include "gl46_window.hpp"
#include "input/keys.hpp"

#include "stb_image.h"
#include "editor/engine_interface.hpp"


namespace Engine
{
bool GL46_Window::Init( const glm::uvec2& size, const std::string& title, const uint32_t flags )
{
	m_screenSize = size;
	m_title      = title;
	if (!glfwInit())
		throw std::runtime_error("Failed to initialize GLFW3");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

	const bool fullscreen = flags & EngineWindowFlags_Fullscreen;
	const bool borderless = flags & EngineWindowFlags_WindowedBorderless;

	if (fullscreen && borderless)
		throw std::logic_error("A window cannot be both fullscreen and borderless!");

	GLFWmonitor*       monitor = nullptr;
	const GLFWvidmode* mode    = nullptr;

	uint32_t sizeX = size.x;
	uint32_t sizeY = size.y;

	if (fullscreen || borderless)
	{
		monitor = glfwGetPrimaryMonitor();
		mode    = glfwGetVideoMode(monitor);

		sizeX = mode->width;
		sizeY = mode->height;

		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	}

	const bool resizable = !(flags & EngineWindowFlags_NoResize);
	glfwWindowHint(GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);

	const bool decorated = !(flags & EngineWindowFlags_NoDecoration);
	glfwWindowHint(GLFW_DECORATED, decorated ? GLFW_TRUE : GLFW_FALSE);

	m_window = glfwCreateWindow(static_cast<int>(sizeX), static_cast<int>(sizeY), title.c_str(), monitor, nullptr);

	if (!m_window)
	{
		glfwTerminate();
		printf("ANSI_ERR, Failed to create OpenGL GLFW window\n");
		std::cout << ANSI_ERR << "Failed to create OpenGL GLFW window" << std::endl;
		return false;
	}

	// Set window icon
	GLFWimage icon[1];
	icon[0].pixels = stbi_load("assets/HeliosLogo.png", &icon[0].width, &icon[0].height, nullptr, 4);
	glfwSetWindowIcon(m_window, 1, icon);
	stbi_image_free(icon[0].pixels);

	glfwMakeContextCurrent(m_window);

	glfwSetWindowUserPointer(m_window, this);
	glfwSetKeyCallback(m_window, KeyCallbackGlfw);
	glfwSetMouseButtonCallback(m_window, &ButtonCallbackGlfw);

	glfwSetWindowSizeCallback(m_window, ResizeCallbackGlfw);
	glfwSetWindowFocusCallback(m_window, FocusCallbackGlfw);
	glfwSetCursorPosCallback(m_window, MouseMoveCallbackGlfw);
	glfwSetScrollCallback(m_window, MouseScrollCallbackGlfw);

	double x, y;
	glfwGetCursorPos(m_window, &x, &y);
	m_mousePos = glm::vec2(static_cast<float>(x), static_cast<float>(y));

	if (flags & EngineWindowFlags_NoVsync)
		glfwSwapInterval(0);

	if (borderless)
	{
		glfwSetWindowSize(m_window, mode->width, mode->height);
		m_screenSize.x = mode->width;
		m_screenSize.y = mode->height;
		glfwSetWindowMonitor(m_window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
	}

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		glfwTerminate();
		throw std::runtime_error("Failed to initialise GLAD");
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init(nullptr);

	// TODO(Quillan): Need to get rid of this part at some point and move it out of platform specific
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	// Add fonts
	constexpr float fontSize = 16.0f;
	io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto-Medium.ttf", fontSize);

	ImFontConfig config;
	config.MergeMode                         = true;
	config.GlyphMinAdvanceX                  = fontSize * 1.1f;
	config.GlyphOffset                       = {0, fontSize * 0.1f};
	static constexpr ImWchar32 icon_ranges[] = {ICON_MIN_MDI, ICON_MAX_MDI, 0};
	io.Fonts->AddFontFromFileTTF("assets/fonts/materialdesignicons-webfont.ttf", fontSize * 1.1f, &config,
	                             icon_ranges);

	Editor::EditorInterface::Get()->m_codeFont = io.Fonts->AddFontFromFileTTF(
			"assets/fonts/CascadiaCode.ttf", fontSize);

	io.Fonts->Build();

	ImGui::StyleColorsDark();
	// Up to here

	glViewport(0, 0, size.x, size.y);

	m_vendor.assign(reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
	return true;
}


void GL46_Window::PollEvents()
{
	glfwPollEvents();
}


void GL46_Window::SwapBuffers()
{
	// Have to reset the context for ImGui (Otherwise it flickers)
	glfwMakeContextCurrent(m_window);
	glfwSwapBuffers(m_window);
}


void GL46_Window::ClearViewport()
{
	int displayW, displayH;
	glfwGetFramebufferSize(m_window, &displayW, &displayH);
	glViewport(0, 0, displayW, displayH);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


int GL46_Window::GetMouseButton( MouseButton button )
{
	return glfwGetMouseButton(m_window, ConvertButtonGlfw(button));
}


int GL46_Window::GetKey( Key key )
{
	return glfwGetKey(m_window, ConvertKeyGlfw(key));
}


glm::uvec2 GL46_Window::GetSize() const
{
	return m_screenSize;
}


const std::string& GL46_Window::GetTitle() const
{
	return m_title;
}


void GL46_Window::SetTitle( const std::string& title )
{
	m_title = title;
	glfwSetWindowTitle(m_window, title.c_str());
}


CursorMode GL46_Window::GetCursorMode()
{
	switch (glfwGetInputMode(m_window, GLFW_CURSOR))
	{
	case GLFW_CURSOR_NORMAL:
		return CursorMode::NORMAL;
	case GLFW_CURSOR_DISABLED:
		return CursorMode::DISABLED;
	case GLFW_CURSOR_HIDDEN:
		return CursorMode::HIDDEN;
	default:
		return CursorMode::NORMAL;
	}
}


void GL46_Window::SetCursorMode( const CursorMode mode )
{
	int glfwMode = GLFW_CURSOR_NORMAL;
	switch (mode)
	{
	case CursorMode::NORMAL:
		glfwMode = GLFW_CURSOR_NORMAL;
		break;
	case CursorMode::HIDDEN:
		glfwMode = GLFW_CURSOR_HIDDEN;
		break;
	case CursorMode::DISABLED:
		glfwMode = GLFW_CURSOR_DISABLED;
		break;
	}
	glfwSetInputMode(m_window, GLFW_CURSOR, glfwMode);
}


const std::string& GL46_Window::GetVendor() const
{
	return m_vendor;
}


void GL46_Window::RequestClose()
{
	glfwSetWindowShouldClose(m_window, true);
}


bool GL46_Window::ShouldClose()
{
	return glfwWindowShouldClose(m_window);
}


void GL46_Window::SetMaximized( const bool maximized )
{
	if (maximized)
		glfwMaximizeWindow(m_window);
	else
		glfwRestoreWindow(m_window);
}


void GL46_Window::Terminate()
{
	glfwDestroyWindow(m_window);
	glfwTerminate();
}


void GL46_Window::KeyCallbackGlfw( GLFWwindow* w, int key, int, int action, int )
{
	auto*     win = static_cast<GL46_Window*>(glfwGetWindowUserPointer(w));
	const Key k   = ConvertGlfwKey(key);
	if (action == GLFW_PRESS)
		CALL(win->onKeyDown, *win, k);
	if (action == GLFW_RELEASE)
		CALL(win->onKeyUp, *win, k);
}


void GL46_Window::ButtonCallbackGlfw( GLFWwindow* w, int button, int action, int )
{
	auto*             win = static_cast<GL46_Window*>(glfwGetWindowUserPointer(w));
	const MouseButton b   = ConvertGlfwButton(button);
	if (action == GLFW_PRESS)
		CALL(win->onMouseDown, *win, b);
	if (action == GLFW_RELEASE)
		CALL(win->onMouseUp, *win, b);
}


void GL46_Window::ResizeCallbackGlfw( GLFWwindow* w, int width, int height )
{
	auto* win         = static_cast<GL46_Window*>(glfwGetWindowUserPointer(w));
	win->m_screenSize = glm::uvec2(width, height);
	CALL(win->onResize, *win, glm::uvec2(width,height));
}


void GL46_Window::FocusCallbackGlfw( GLFWwindow* w, int f )
{
	auto* win = static_cast<GL46_Window*>(glfwGetWindowUserPointer(w));
	CALL(win->onFocus, *win, f == GLFW_TRUE);
}


void GL46_Window::MouseMoveCallbackGlfw( GLFWwindow* w, double x, double y )
{
	auto*           win = static_cast<GL46_Window*>(glfwGetWindowUserPointer(w));
	const glm::vec2 diff{
			static_cast<float>(x) - win->m_mousePos.x,
			static_cast<float>(y) - win->m_mousePos.y
	};
	CALL(win->onMouseMove, *win, diff);

	win->m_mousePos = glm::vec2(static_cast<float>(x), static_cast<float>(y));
}


void GL46_Window::MouseScrollCallbackGlfw( GLFWwindow* w, double x, double y )
{
	auto* win = static_cast<GL46_Window*>(glfwGetWindowUserPointer(w));
	CALL(win->onMouseScroll, *win, static_cast<float>(x), static_cast<float>(y));
}
} // Engine
