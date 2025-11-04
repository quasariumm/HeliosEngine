#pragma once

#include "ImGuizmo.h"
#include "editor/engine_interface.hpp"


// Forward declarations
namespace Engine
{
// ReSharper disable once CppInconsistentNaming
class GL46_Texture2D;
}


// ReSharper disable once CppInconsistentNaming
using GLuint = unsigned int;


namespace Engine::Editor
{
// Editor menu is the bar at the top of the entire editor
class EditorMenu
{
public:

	static void Draw();
};


REGISTER_INTERFACE(EditorMenu);


// Inspector shows information about currently selected object
class EntityInspector
{
public:

	static void Draw();
};


REGISTER_INTERFACE(EntityInspector);


// Scene graph shows the hierarchy of the scene
class SceneGraph
{
public:

	static void Draw();

	static void DrawObjectGraph( SceneObject entity );

	static SceneObject selected;
};


REGISTER_INTERFACE(SceneGraph);


// Log viewer shows any logs made using the Log namespace
class LogViewer
{
public:

	static void Draw();
};


REGISTER_INTERFACE(LogViewer);


// Viewport shows the rendered scene
class Viewport
{
public:

	void Draw();

	void DrawOverlay();
	void DrawGizmos() const;

	void SetRenderImage( GL46_Texture2D* image ) { m_renderedImage = image; }


	[[nodiscard]]
	glm::uvec2 GetViewportSize() const { return m_viewportSize; }


	bool drawStatistics = true;

private:

	GL46_Texture2D* m_renderedImage = nullptr;

	glm::uvec2 m_viewportSize{1920u, 1080u};

	ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
	float snapGrid = 0.1f;
	bool snapping = false;
};
}
