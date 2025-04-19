#include "Camera.h"

#include "Core/Window.h"


namespace Engine
{

/*
 * Class: Camera
 */

Camera::Camera()
{
	// Set up a basic view frustum
	m_camPos = vec3f( 0, 0, -2 );

	m_camFront = vec3f( 0, 0, 1 );
	m_camRight = Normalise( Cross( m_worldUp, m_camFront ) );
	m_camUp = Normalise( Cross( m_camFront, m_camRight ) );

	UpdateCameraVectors();
}


vec3f Camera::GetViewportParameters( const vec2u& viewportSize )
{
	// Viewport size
	const float viewportHeight = 2.f * std::tan(RAD(0.25f * m_fov)) * m_focusPlaneDistance;
	const float viewportWidth = viewportHeight * (static_cast<float>(viewportSize.x) / static_cast<float>(viewportSize.y));
	return { viewportWidth, viewportHeight, m_focusPlaneDistance };
}


mat4f Camera::GetViewMatrix() const
{
	return LookAt(m_camPos, m_camFront, m_camUp);
}


mat4f Camera::GetProjectionMatrix(const vec2u& viewportSize) const
{
	return Perspective(m_fov, static_cast<float>(viewportSize.x) / static_cast<float>(viewportSize.y), 0.1f, 100.0f);
}


mat4f Camera::GetCamToWorldMatrix( const vec2u& viewportSize ) const
{
	return mat4f(
	{ m_camRight.x, 	m_camRight.y, 	m_camRight.z, 	m_camPos.x },
	{ -m_camUp.x, 	-m_camUp.y, 	-m_camUp.z, 		m_camPos.y },
	{ m_camFront.x, 	m_camFront.y, 	m_camFront.z, 	m_camPos.z },
	{ 0, 				0, 			0, 			0 }
	);
}


void Camera::HandleInput( Window& window, const float deltaTime )
{
	if (m_usingCamera)
		window.SetCursorMode(CursorMode::DISABLED);
	else
	{
		window.SetCursorMode(CursorMode::NORMAL);
		return;
	}

	float speed = 20.f * deltaTime;

	if (window.GetKey( Key::LEFT_SHIFT )) speed *= 5.0f;
	if (window.GetKey( Key::LEFT_CONTROL )) speed *= 0.2f;

	UpdateCameraVectors();
	bool changed = false;
	if (window.GetKey( Key::A )) m_camPos -= m_camRight * speed, changed = true;
	if (window.GetKey( Key::D )) m_camPos += m_camRight * speed, changed = true;
	if (window.GetKey( Key::W )) m_camPos += m_camFront * speed, changed = true;
	if (window.GetKey( Key::S )) m_camPos -= m_camFront * speed, changed = true;
	if (window.GetKey( Key::R )) m_camPos += m_camUp * speed, changed = true;
	if (window.GetKey( Key::F )) m_camPos -= m_camUp * speed, changed = true;
	if (!changed) return;
	UpdateCameraVectors();
}


void Camera::MouseMove( const vec2f& delta )
{
	if (!m_usingCamera)
		return;

	m_yaw   += 0.1f * delta.x;
	m_pitch += -0.1f * delta.y;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	m_pitch = std::clamp(m_pitch, -89.9f, 89.9f);

	// update Front, Right and Up Vectors using the updated Euler angles
	UpdateCameraVectors();
}

void Camera::MouseButtonDown(const MouseButton& button)
{
	if (button == MouseButton::RIGHT)
		m_usingCamera = true;
}

void Camera::MouseButtonUp(const MouseButton& button)
{
	if (button == MouseButton::RIGHT)
		m_usingCamera = false;
}


void Camera::UpdateCameraVectors()
{
	// calculate the new Front vector
	vec3f front;
	front.x = std::cos(RAD(m_yaw)) * std::cos(RAD(m_pitch));
	front.y = std::sin(RAD(m_pitch));
	front.z = std::sin(RAD(m_yaw)) * std::cos(RAD(m_pitch));
	m_camFront = Normalise(front);
	// also re-calculate the Right and Up vector
	m_camRight = Normalise(Cross(m_camFront, m_worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	m_camUp    = Normalise(Cross(m_camRight, m_camFront));
}

} // Engine