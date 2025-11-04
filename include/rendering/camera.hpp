#pragma once
#include "input/keys.hpp"


namespace Helios
{
class Window;


class Camera
{
public:

	Camera();

	/**
	 * @brief Handles the keyboard input for the camera
	 * @param window The window
	 * @param deltaTime The time since the last frame
	 */
	void HandleInput( Window& window, float deltaTime );

	/**
	 * @brief Handles the mouse movement for the camera
	 * @param delta The distance the cursor has traveled
	 */
	void MouseMove( const glm::vec2& delta );

	/**
	 * @brief Handles button presses for the camera
	 * @param button The button that has been pressed
	 */
	void MouseButtonDown( const MouseButton& button );

	/**
	 * @brief Handles button release for the camera
	 * @param button The button that has been released
	 */
	void MouseButtonUp( const MouseButton& button );

	/**
	 * @brief Gives the viewport parameters used for raytracing
	 * @param viewportSize The size in pixels of the raytracing texture
	 * @return The viewport width and height in coordinate units and the focal plane distance
	 */
	[[nodiscard]]
	glm::vec3 GetViewportParameters( const glm::uvec2& viewportSize ) const;

	/**
	 * @brief Gives a matrix that converts world space to view space
	 * @return The view matrix
	 */
	[[nodiscard]]
	glm::mat4 GetViewMatrix() const;

	/**
	 * @brief Gives a matrix that converts view space to screen space coordinates
	 * @param viewportSize The size in pixels of the raytracing texture
	 * @return The projection matrix
	 */
	[[nodiscard]]
	glm::mat4 GetProjectionMatrix( const glm::uvec2& viewportSize ) const;

	/**
	 * @brief Gives a matrix that converts
	 * @return The camera to world matrix
	 */
	[[nodiscard]]
	glm::mat4 GetCamToWorldMatrix() const;


	[[nodiscard]]
	bool LockedToViewport() const { return m_usingCamera; }


	void SetAllowUseControls( bool allow ) { m_allowUsingCamera = allow; }


	[[nodiscard]]
	glm::vec3 GetCameraPosition() const { return m_camPos; }


	[[nodiscard]]
	glm::vec3 GetCameraRightVec() const { return m_camRight; }


	[[nodiscard]]
	glm::vec3 GetCameraFrontVec() const { return m_camFront; }


	[[nodiscard]]
	glm::vec3 GetCameraUpVec() const { return m_camUp; }

private:

	const glm::vec3 m_worldUp = glm::vec3(0, 1, 0);
	glm::vec3       m_camPos{};
	float           m_yaw   = -90.f;
	float           m_pitch = 0.f;
	glm::vec3       m_camFront{}, m_camUp{}, m_camRight{};
	float           m_fov = 70.0f;
	// Used for Ray tracing
	float m_focusPlaneDistance = 1.f;

	bool m_usingCamera      = false;
	bool m_allowUsingCamera = true;

	/**
	 * @brief Updates the front, right and up vectors of the camera
	 */
	void UpdateCameraVectors();
};
} // Engine
