#pragma once
#include "../../../include/rendering/window.hpp"


// ReSharper disable once CppInconsistentNaming
struct GLFWwindow;

namespace Engine
{

// ReSharper disable once CppInconsistentNaming
class GL46_Window final : public Window
{

public:

    GL46_Window() = default;

    bool Init(const glm::uvec2& size, const std::string& title, uint32_t flags) override;

    void PollEvents() override;
	void SwapBuffers() override;
    void ClearViewport() override;
    int GetMouseButton(MouseButton button) override;
    int GetKey(Key key) override;

	[[nodiscard]]
	glm::uvec2 GetSize() const override;

	[[nodiscard]]
	const std::string& GetTitle() const override;

	void SetTitle(const std::string& title) override;

	[[nodiscard]]
	CursorMode GetCursorMode() override;

	void SetCursorMode(CursorMode mode) override;

    /**
     * @brief Gets the vendor from OpenGL
     * @note This in mostly used for internal purposes
     */
	[[nodiscard]]
    const std::string& GetVendor() const;

	void RequestClose() override;

    bool ShouldClose() override;

	void SetMaximized(bool maximized) override;

    void Terminate();

private:

    GLFWwindow* m_window{};

	glm::uvec2 m_screenSize{};
	glm::vec2 m_mousePos{};

	std::string m_vendor;

    static void ResizeCallbackGlfw(GLFWwindow* w, int width, int height);
    static void FocusCallbackGlfw(GLFWwindow* w, int f);

    static void KeyCallbackGlfw(GLFWwindow* w, int key, int, int action, int);

    static void ButtonCallbackGlfw(GLFWwindow* w, int button, int action, int);
    static void MouseMoveCallbackGlfw(GLFWwindow* w, double x, double y);
    static void MouseScrollCallbackGlfw(GLFWwindow* w, double x, double y);

};

} // Engine
