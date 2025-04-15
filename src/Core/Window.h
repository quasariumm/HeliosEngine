#pragma once

#include "Keys.h"
#include <functional>
#include <memory>

#include "Math/Vector.h"
#include <string>


#define EngineWindowFlags_None					0x00000000u
#define EngineWindowFlags_NoVsync 				0x00000001u
#define EngineWindowFlags_NoResize 				0x00000002u
#define EngineWindowFlags_NoDecoration 			0x00000004u
#define EngineWindowFlags_Fullscreen			0x00000008u
#define EngineWindowFlags_WindowedBorderless	0x00000010u


namespace Engine
{

enum class GraphicsAPI
{
    NONE,
    VULKAN,
    DIRECTX12,
    OPENGL
};

class Window
{

public:

    typedef std::function<void(Window&, const vec2u&)>  resizeCallback_t;
    typedef std::function<void(Window&, bool)>          focusCallback_t;
    typedef std::function<void(Window&, vec2f)>  		mouseMoveCallback_t;
    typedef std::function<void(Window&, MouseButton)>   mouseButtonCallback_t;
    typedef std::function<void(Window&, float, float)>  mouseScrollCallback_t;
    typedef std::function<void(Window&, Key)>           keyCallback_t;

    Window() = default;

    virtual bool Init(const vec2u& size, const std::wstring& title, uint32_t flags) = 0;

    virtual void PollEvents() = 0;
	virtual void SwapBuffers() = 0;
	virtual void ClearViewport() = 0;
    virtual bool ShouldClose() = 0;

    virtual int GetMouseButton(MouseButton button) = 0;
    virtual int GetKey(Key key) = 0;

	[[nodiscard]]
	virtual vec2u GetSize() const = 0;

	[[nodiscard]]
	virtual const std::wstring& GetTitle() const = 0;

	virtual void SetTitle(const std::wstring& title) = 0;

	virtual void SetShouldClose(bool shouldClose) = 0;

    /*
     * Window callbacks
     */

    void SetResizeCallback(const resizeCallback_t& callback);
    void SetFocusCallback(const focusCallback_t& callback);

    void SetMouseMoveCallback(const mouseMoveCallback_t& callback);

    void SetMouseButtonDownCallback(const mouseButtonCallback_t& callback);
    void SetMouseButtonUpCallback(const mouseButtonCallback_t& callback);
    void SetMouseScrollCallback(const mouseScrollCallback_t& callback);

    void SetKeyDownCallback(const keyCallback_t& callback);
    void SetKeyUpCallback(const keyCallback_t& callback);

protected:
    GraphicsAPI m_currentAPI = GraphicsAPI::NONE;
	std::wstring m_title;

    resizeCallback_t onResize;
    focusCallback_t onFocus;

    mouseMoveCallback_t onMouseMove;
    mouseButtonCallback_t onMouseDown;
    mouseButtonCallback_t onMouseUp;
    mouseScrollCallback_t onMouseScroll;

    keyCallback_t onKeyDown;
    keyCallback_t onKeyUp;
};

void CreateWin(
	std::unique_ptr<Window>& window,
	const vec2u& size, const std::wstring& title,
	uint32_t flags = EngineWindowFlags_None,
	GraphicsAPI api = GraphicsAPI::OPENGL
);

} // Engine
