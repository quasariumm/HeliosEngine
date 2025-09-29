#pragma once

#include "Keys.h"


static constexpr uint32_t EngineWindowFlags_None				= 0x00000000u;
static constexpr uint32_t EngineWindowFlags_NoVsync 			= 0x00000001u;
static constexpr uint32_t EngineWindowFlags_NoResize 			= 0x00000002u;
static constexpr uint32_t EngineWindowFlags_NoDecoration 		= 0x00000004u;
static constexpr uint32_t EngineWindowFlags_Fullscreen			= 0x00000008u;
static constexpr uint32_t EngineWindowFlags_WindowedBorderless	= 0x00000010u;


namespace Engine
{

// Predefine the windows for all the platforms
class GL46_Window;
class DX12_Window;
class VK_Window;

enum class CursorMode
{
	NORMAL,
	HIDDEN,
	DISABLED
};

class Window
{

public:

    using resizeCallback_t		= std::function<void(Window&, const vec2u&)>;
    using focusCallback_t		= std::function<void(Window&, bool)>;
    using mouseMoveCallback_t	= std::function<void(Window&, vec2f)>;
    using mouseButtonCallback_t = std::function<void(Window&, MouseButton)>;
    using mouseScrollCallback_t = std::function<void(Window&, float, float)>;
    using keyCallback_t			= std::function<void(Window&, Key)>;

    Window() = default;
	virtual ~Window() = default;

	auto* GetPlatformPtr()
	{
#if defined HELIOS_API_GL46
		return reinterpret_cast<GL46_Window*>(this);
#elif defined HELIOS_API_DX12
		return reinterpret_cast<DX12_Window*>(this);
#elif defined HELIOS_API_VK
		return reinterpret_cast<VK_Window*>(this);
#else
		return nullptr;
#endif
	}

    virtual bool Init(const vec2u& size, const std::wstring& title, uint32_t flags) = 0;

    virtual void PollEvents() = 0;
	virtual void BeginFrame() {} /* OpenGL does not need this function, but modern APIs do */
	virtual void SwapBuffers() = 0;
	virtual void ClearViewport() = 0;
    virtual bool ShouldClose() = 0;

    virtual int GetMouseButton(MouseButton button) = 0;
    virtual int GetKey(Key key) = 0;

	[[nodiscard]]
	virtual vec2u GetSize() const = 0;

	[[nodiscard]]
	virtual const std::wstring& GetTitle() const;

	virtual void SetTitle(const std::wstring& title) = 0;

	[[nodiscard]]
	virtual CursorMode GetCursorMode() = 0;

	virtual void SetCursorMode(CursorMode mode) = 0;

	virtual void RequestClose();

	virtual void SetMaximized(bool maximized) = 0;

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
	bool m_initialized = false;
	bool m_shouldClose = false;
	bool m_vsync = false;
	bool m_fullscreen = false;
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
	uint32_t flags = EngineWindowFlags_None
);

} // Engine
