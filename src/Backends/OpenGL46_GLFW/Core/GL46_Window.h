#pragma once
#include "Core/Window.h"
#include "Math/Vector.h"


class GLFWwindow;

namespace Engine
{

class GL46_Window : public Window
{

public:

    GL46_Window() = default;

    bool Init(const vec2u& size, const std::wstring& title) override;

    void PollEvents() override;
	void SwapBuffers() override;
    void ClearViewport() override;
    int GetMouseButton(MouseButton button) override;
    int GetKey(Key key) override;

	vec2u GetSize() const override;

    bool ShouldClose() override;

    void Terminate();

private:

    GLFWwindow* m_window;

	vec2u m_screenSize;

    static void ResizeCallbackGLFW(GLFWwindow* w, int width, int height);
    static void FocusCallbackGLFW(GLFWwindow* w, int f);

    static void KeyCallbackGLFW(GLFWwindow* w, int key, int scancode, int action, int mods);

    static void ButtonCallbackGLFW(GLFWwindow* w, int button, int action, int mods);
    static void MouseMoveCallbackGLFW(GLFWwindow* w, double x, double y);
    static void MouseScrollCallbackGLFW(GLFWwindow* w, double x, double y);

};

} // Engine
