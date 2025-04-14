#include "GL46_Window.h"
#include "Core/Common.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Core/Keys.h"

#include <cstdlib>
#include <iostream>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_opengl3_loader.h>

namespace Engine
{

bool GL46_Window::Init(const vec2u& size, const std::wstring& title)
{
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW3");
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const std::string titleUTF8 = WStringToUTF8(title);

    m_window = glfwCreateWindow(size.x, size.y, titleUTF8.c_str(), nullptr, nullptr);

    if (!m_window)
    {
        glfwTerminate();
        printf("ANSI_ERR, Failed to create OpenGL GLFW window\n");
        std::wcout << ANSI_ERR << "Failed to create OpenGL GLFW window" << std::endl;
        return false;
    }

    glfwMakeContextCurrent( m_window );

    glfwSetWindowUserPointer( m_window, this );
    glfwSetKeyCallback( m_window, KeyCallbackGLFW );
    glfwSetMouseButtonCallback( m_window, &ButtonCallbackGLFW) ;

    glfwSetWindowSizeCallback( m_window, ResizeCallbackGLFW );
    glfwSetWindowFocusCallback( m_window,  FocusCallbackGLFW );
    glfwSetCursorPosCallback( m_window, MouseMoveCallbackGLFW ) ;
    glfwSetScrollCallback( m_window, MouseScrollCallbackGLFW );

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        throw std::runtime_error("Failed to initialise GLAD");
    }

    glViewport(0, 0, size.x, size.y);

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init(0);
    ImGui::StyleColorsDark();
    ImGuiIO& io    = ImGui::GetIO();
    io.IniFilename = "./imgui.ini";

    m_currentAPI = GraphicsAPI::OPENGL;
    return true;
}


void GL46_Window::PollEvents()
{
    glfwPollEvents();
}

int GL46_Window::GetMouseButton(MouseButton button)
{
    return glfwGetMouseButton( m_window, ConvertButtonGLFW(button) );
}

int GL46_Window::GetKey(Key key)
{
    glfwGetKey( m_window, ConvertKeyGLFW(key) );
}


bool GL46_Window::ShouldClose()
{
    return glfwWindowShouldClose(m_window);
}


void GL46_Window::Terminate()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void GL46_Window::KeyCallbackGLFW(GLFWwindow* w, int key, int scancode, int action, int mods)
{
    GL46_Window* win = static_cast<GL46_Window*>(glfwGetWindowUserPointer(w));
    Key k = ConvertGLFWKey( key );
    if( action == GLFW_PRESS )
        win->onKeyDown(*win, k);
    if( action == GLFW_RELEASE )
        win->onKeyUp(*win, k);
}


void GL46_Window::ButtonCallbackGLFW(GLFWwindow* w, int button, int action, int mods)
{
    GL46_Window* win = static_cast<GL46_Window*>(glfwGetWindowUserPointer(w));
    MouseButton b = ConvertGLFWButton( button );
    if( action == GLFW_PRESS )
        win->onMouseDown(*win, b);
    if( action == GLFW_RELEASE )
        win->onMouseUp(*win, b);
}

void GL46_Window::ResizeCallbackGLFW(GLFWwindow* w, int width, int height)
{
    GL46_Window* win = static_cast<GL46_Window*>(glfwGetWindowUserPointer(w));
    win->onResize(*win,vec2u(width,height));
}

void GL46_Window::FocusCallbackGLFW(GLFWwindow* w, int f)
{
    GL46_Window* win = static_cast<GL46_Window*>(glfwGetWindowUserPointer(w));
    win->onFocus(*win, f == GLFW_TRUE);
}

void GL46_Window::MouseMoveCallbackGLFW(GLFWwindow* w, double x, double y)
{
    GL46_Window* win = static_cast<GL46_Window*>(glfwGetWindowUserPointer(w));
    win->onMouseMove(*win,vec2u(static_cast<uint32_t>(x),static_cast<uint32_t>(y)));
}

void GL46_Window::MouseScrollCallbackGLFW(GLFWwindow* w, double x, double y)
{
    GL46_Window* win = static_cast<GL46_Window*>(glfwGetWindowUserPointer(w));
    win->onMouseScroll(*win,static_cast<float>(x),static_cast<float>(y));
}
} // Engine