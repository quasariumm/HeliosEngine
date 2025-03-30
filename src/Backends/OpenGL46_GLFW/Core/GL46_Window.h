#pragma once
#include "Math/Vector.h"

class GLFWwindow;

namespace Engine
{

class GL46_Window 
{

public:

    GL46_Window() = default;

    void Init(const vec2u& size, const std::wstring& title);

    void PollEvents();

    bool ShouldClose();

    void Terminate();

private:

    GLFWwindow* m_window;

    void KeyCallback(int key, int scancode, int action, int mods);
    void ButtonCallback(int button, int action, int mods);

};

} // Engine
