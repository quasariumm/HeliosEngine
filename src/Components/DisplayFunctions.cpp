#include "DisplayFunctions.h"
#include "imgui.h"
#include "Component.h"


namespace Engine
{


void DisplayNone(const ComponentProperty* property)
{
	// Do nothing
}


void DisplayColorRGB(const ComponentProperty* property)
{
	ImGui::ColorEdit3(WStringToUTF8(property->name).c_str(), static_cast<vec3*>(property->value)->cell);
}


void DisplayColorRGB_HDR( const ComponentProperty* property )
{
	ImGui::ColorEdit3(WStringToUTF8(property->name).c_str(), static_cast<vec3*>(property->value)->cell, ImGuiColorEditFlags_HDR);
}


void DisplayColorHSV( const ComponentProperty* property )
{
	ImGui::ColorEdit3(WStringToUTF8(property->name).c_str(), static_cast<vec3*>(property->value)->cell, ImGuiColorEditFlags_DisplayHSV);
}


void DisplayColorHSV_HDR( const ComponentProperty* property )
{
	ImGui::ColorEdit3(WStringToUTF8(property->name).c_str(), static_cast<vec3*>(property->value)->cell, ImGuiColorEditFlags_DisplayHSV | ImGuiColorEditFlags_HDR);
}


void DisplayColorRGBA(const ComponentProperty* property)
{
	ImGui::ColorEdit4(WStringToUTF8(property->name).c_str(), static_cast<vec4*>(property->value)->cell);
}


void DisplayColorRGBA_HDR( const ComponentProperty* property )
{
	ImGui::ColorEdit4(WStringToUTF8(property->name).c_str(), static_cast<vec4*>(property->value)->cell, ImGuiColorEditFlags_HDR);
}


void DisplayColorHSVA( const ComponentProperty* property )
{
	ImGui::ColorEdit4(WStringToUTF8(property->name).c_str(), static_cast<vec4*>(property->value)->cell, ImGuiColorEditFlags_DisplayHSV);
}


void DisplayColorHSVA_HDR( const ComponentProperty* property )
{
	ImGui::ColorEdit4(WStringToUTF8(property->name).c_str(), static_cast<vec4*>(property->value)->cell, ImGuiColorEditFlags_DisplayHSV | ImGuiColorEditFlags_HDR);
}

}
