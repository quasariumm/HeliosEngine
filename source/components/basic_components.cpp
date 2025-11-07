#include "components/basic_components.hpp"

#include "tools/editor_widgets.hpp"
#include "tools/icon_defines.hpp"

using namespace Helios;
using namespace Components;


void Transform::Inspector()
{
	ImVec4 red   = ImVec4(1.0f, 0.0f, 0.0f, 0.5f);
	ImVec4 green = ImVec4(0.0f, 1.0f, 0.0f, 0.5f);
	ImVec4 blue  = ImVec4(0.0f, 0.0f, 1.0f, 0.5f);

	bool modified = false;

	ImGui::Text(ICON_AXIS_ARROW);
	modified |= ImGui::ColoredFloatInput("X", &PositionRef()->x, red, "PositionX", true);
	modified |= ImGui::ColoredFloatInput("Y", &PositionRef()->y, green, "PositionY", true);
	modified |= ImGui::ColoredFloatInput("Z", &PositionRef()->z, blue, "PositionZ", true);

	glm::vec3 rot     = degrees(GetEulerRotation());
	bool      changed = false;

	ImGui::Text(ICON_ROTATE_ORBIT);
	changed |= ImGui::ColoredFloatInput("X", &rot.x, red, "RotationX", true);
	changed |= ImGui::ColoredFloatInput("Y", &rot.y, green, "RotationY", true);
	changed |= ImGui::ColoredFloatInput("Z", &rot.z, blue, "RotationZ", true);

	if (changed)
		SetEulerRotation(rot);
	modified |= changed;

	ImGui::Text(ICON_RESIZE);
	modified |= ImGui::ColoredFloatInput("X", &ScaleRef()->x, red, "ScaleX", true);
	modified |= ImGui::ColoredFloatInput("Y", &ScaleRef()->y, green, "ScaleY", true);
	modified |= ImGui::ColoredFloatInput("Z", &ScaleRef()->z, blue, "ScaleZ", true);

	if (modified && onModifiedUsed)
		onModified();
}


// TODO(Quillan): Pls make thhis thhe way you want to
// void Camera::Inspector()
// {
//     ImGui::InputFloat("FOV", &fov);
//     ImGui::InputFloat("Near Plane", &near_plane);
//     ImGui::InputFloat("Far Plane", &far_plane);
// }
