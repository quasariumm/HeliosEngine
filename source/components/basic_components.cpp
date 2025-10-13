#include "components/basic_components.hpp"

#include "tools/Icon_defines.hpp"

using namespace Engine;
using namespace Components;

static bool ColoredFloatInput(const std::string& label, float* v, ImVec4 color, const char* id, bool same_line)
{
    ImGui::PushID(id);
    if (same_line)
        ImGui::SameLine();
    ImGui::Text("%s", label.c_str());
    ImGui::SameLine();
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, {0,0,0,0});
    ImGui::PushStyleColor(ImGuiCol_Border, color);
    ImGui::SetNextItemWidth(4 * ImGui::GetFontSize());
    bool changed = ImGui::DragFloat("", v, 0.001f);
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);
    ImGui::PopID();
    return changed;
}

void Transform::Inspector()
{
    ImVec4 red = ImVec4(1.0f, 0.0f, 0.0f, 0.5f);
    ImVec4 green = ImVec4(0.0f, 1.0f, 0.0f, 0.5f);
    ImVec4 blue = ImVec4(0.0f, 0.0f, 1.0f, 0.5f);

    bool modified = false;

    ImGui::Text(ICON_AXIS_ARROW);
    modified |= ColoredFloatInput("X", &PositionRef()->x, red, "PositionX", true );
    modified |= ColoredFloatInput("Y", &PositionRef()->y, green, "PositionY", true );
    modified |= ColoredFloatInput("Z", &PositionRef()->z, blue, "PositionZ", true );

    glm::vec3 rot = degrees(GetEulerRotation());
    bool changed = false;
        
    ImGui::Text(ICON_ROTATE_ORBIT);
    changed |= ColoredFloatInput("X", &rot.x, red, "RotationX", true);
    changed |= ColoredFloatInput("Y", &rot.y, green, "RotationY", true );
    changed |= ColoredFloatInput("Z", &rot.z, blue, "RotationZ", true );

    if (changed) SetEulerRotation(rot);
    modified |= changed;

    ImGui::Text(ICON_RESIZE);
    modified |= ColoredFloatInput("X", &ScaleRef()->x, red, "ScaleX", true );
    modified |= ColoredFloatInput("Y", &ScaleRef()->y, green, "ScaleY", true );
    modified |= ColoredFloatInput("Z", &ScaleRef()->z, blue, "ScaleZ", true );

    if (modified && onModifiedUsed)
        onModified();
}

void DebugCube::Inspector()
{
    ImGui::ColorPicker3("Color", value_ptr(color));
}

void Camera::Inspector()
{
    ImGui::InputFloat("FOV", &fov);
    ImGui::InputFloat("Near Plane", &near_plane);
    ImGui::InputFloat("Far Plane", &far_plane);
}
