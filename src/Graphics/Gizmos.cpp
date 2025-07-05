#include "Gizmos.h"

#include "Camera.h"
#include "Debugger/Debugger.h"
#include "Viewport/Viewport.h"


namespace Engine
{

Camera* Gizmos::m_camera = nullptr;
Viewport* Gizmos::m_viewport = nullptr;

void Gizmos::Init( Viewport& viewport, Camera& camera )
{
	m_viewport = &viewport;
	m_camera = &camera;
}


Line2D Gizmos::ProjectLine( const Line3D& line )
{
	return { ProjectPoint(line.a), ProjectPoint(line.b) };
}


void Gizmos::DrawLine( const Line3D& line, const Color32F color, const float thickness )
{
	const Line2D projectedLine = ProjectLine( line );

	const ImVec2 viewportSize = m_viewport->size;

	const ImVec2 p0(
		(projectedLine.a.x + 1.f) * 0.5f * viewportSize.x,
		(projectedLine.a.y + 1.f) * 0.5f * viewportSize.y
	);
	const ImVec2 p1(
		(projectedLine.b.x + 1.f) * 0.5f * viewportSize.x,
		(projectedLine.b.y + 1.f) * 0.5f * viewportSize.y
	);

	ImDrawList* drawList = ImGui::GetForegroundDrawList();
	drawList->AddLine(p0, p1, IM_COL32(color.r, color.g, color.b, color.a), thickness);
}


ImVec2 Gizmos::ProjectPoint( const vec3& point )
{
	if (m_camera == nullptr)
	{
		DebugLog(LogSeverity::SEVERE, L"[Gizmos::ProjectPoint] Gizmos have not been initialised. Please call Init() before you call any of the gizmo functions");
		return {};
	}

	const mat4f worldToCam = Invert(m_camera->GetCamToWorldMatrix());

	vec3f projected = TransformPoint(point, worldToCam);

	return {projected.x, projected.y};
}


void Gizmos::DrawPoint( const vec3& point )
{
}

} // Engine