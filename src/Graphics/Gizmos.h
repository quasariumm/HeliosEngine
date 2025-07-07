#pragma once

namespace Engine
{
class Camera;
class Viewport;
}

namespace Engine
{

struct alignas(8) Line3D
{
	vec3 a, b;
};

struct alignas(16) Line2D
{
	ImVec2 a, b;
};

class Gizmos
{
public:

	static void Init(Camera& camera);

	//static void DrawGizmo(const vec3& pos, float size = 30.0f);

	/**
	 * @brief Projects a world space line to screen space
	 * @param line The line you want to project
	 * @return The projected line in screen space (0 to 1)
	 */
	static Line2D ProjectLine(const Line3D& line);

	/**
	 * @brief Projects and draws a line on the screen
	 * @param line The line you want to draw projected onto the viewport
	 * @param color The color of the line
	 * @param thickness The thickness of the line
	 */
	static void DrawLine(const Line3D& line, Color32F color, float thickness = 1.f);

	static ImVec2 ProjectPoint(const vec3& point);
	static void DrawPoint(const vec3& point);

private:

	static Camera* m_camera;

};

} // Engine
