#pragma once
#include <string>

#include "imgui.h"
#include "glm/gtc/type_ptr.inl"

namespace ImGui
{

inline bool InputVec3(const char* label, glm::vec3& value, const char* format = "%.1", ImGuiInputTextFlags flags = 0)
{
	return InputFloat3(label, value_ptr(value), format, flags);
}

inline bool ColorEdit(const char* label, glm::vec4& value)
{
	return ColorPicker4(label, value_ptr(value), ImGuiColorEditFlags_DisplayHSV);
}

inline bool SliderFloatSimple(const char* label, float* value)
{
	return SliderFloat(label, value, 0.0f, 1.0f, "%.2f");
}

inline bool ColoredFloatInput(const std::string& label, float* v, const ImVec4 color, const char* id, bool same_line)
{
	PushID(id);
	if (same_line)
		SameLine();
	Text("%s", label.c_str());
	SameLine();
	PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
	PushStyleColor(ImGuiCol_FrameBg, {0,0,0,0});
	PushStyleColor(ImGuiCol_Border, color);
	SetNextItemWidth(4 * GetFontSize());
	bool changed = DragFloat("", v, 0.001f);
	PopStyleVar();
	PopStyleColor(2);
	PopID();
	return changed;
}

}
