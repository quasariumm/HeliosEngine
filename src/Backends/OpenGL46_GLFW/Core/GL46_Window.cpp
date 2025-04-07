#include "GL46_Window.h"
#include "Core/Common.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cstdlib>

namespace Engine
{

void GL46_Window::Init(const vec2u& size, const std::wstring& title)
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
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(m_window);

    // auto keyCallback = [this](GLFWwindow*, const int key, const int scancode, const int action, const int mods)
    //     {
    //         this->KeyCallback(key, scancode, action, mods);
    //     };
    //
    // auto buttonCallback = [this](GLFWwindow*, const int button, const int action, const int mods)
    //     {
    //         this->ButtonCallback(button, action, mods);
    //     };
    //
    // glfwSetKeyCallback(m_window, &keyCallback.operator());
    // glfwSetMouseButtonCallback(m_window, &buttonCallback.operator());

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        throw std::runtime_error("Failed to initialise GLAD");
    }

    glViewport(0, 0, size.x, size.y);
}


void GL46_Window::PollEvents()
{
    glfwPollEvents();
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


void GL46_Window::KeyCallback(int key, int scancode, int action, int mods)
{
    printf("%d", key);
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(m_window, true);
}


void GL46_Window::ButtonCallback(int button, int action, int mods)
{
    printf("%d", button);
}

} // Engine