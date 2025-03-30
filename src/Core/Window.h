#pragma once

#include "Keys.h"
#include <functional>

#include "Math/Vector.h"


namespace Engine
{

class Window 
{

public:

    typedef std::function<void(Window&, const vec2u&)>  resizeCallback_t;
    typedef resizeCallback_t                            mouseMoveCallback_t;
    typedef std::function<void(Window&, MouseButton)>   mouseButtonCallback_t;
    typedef std::function<void(Window&, float, float)>  mouseScrollCallback_t;
    typedef std::function<void(Window&, Key)>           keyCallback_t;

    Window() = default;

    void PollEvents();

    bool GetMouseButton(MouseButton button);
    bool GetKey(Key key);

    /*
     * Window callbacks
     */

    void SetResizeCallback(const resizeCallback_t& callback);

    void SetMouseMoveCallback(const mouseMoveCallback_t& callback);

    void SetMouseButtonDownCallback(const mouseButtonCallback_t& callback);
    void SetMouseButtonUpCallback(const mouseButtonCallback_t& callback);
    void SetMouseScrollCallback(const mouseScrollCallback_t& callback);

    void SetKeyDownCallback(const keyCallback_t& callback);
    void SetKeyUpCallback(const keyCallback_t& callback);

private:

    vec2u m_screenSize;

    resizeCallback_t onResize;

    mouseMoveCallback_t onMouseMove;
    mouseButtonCallback_t onMouseDown;
    mouseButtonCallback_t onMouseUp;

    mouseScrollCallback_t onMouseScroll;

    keyCallback_t onKeyDown;
    keyCallback_t onKeyUp;

};

} // Engine
