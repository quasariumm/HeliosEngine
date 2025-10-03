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
	ImGui::ColorEdit3(WStringToUTF8(property->name).c_str(), glm::value_ptr(*static_cast<glm::vec3*>(property->value)));
}


void DisplayColorRGB_HDR( const ComponentProperty* property )
{
	ImGui::ColorEdit3(WStringToUTF8(property->name).c_str(), glm::value_ptr(*static_cast<glm::vec3*>(property->value)), ImGuiColorEditFlags_HDR);
}


void DisplayColorHSV( const ComponentProperty* property )
{
	ImGui::ColorEdit3(WStringToUTF8(property->name).c_str(), glm::value_ptr(*static_cast<glm::vec3*>(property->value)), ImGuiColorEditFlags_DisplayHSV);
}


void DisplayColorHSV_HDR( const ComponentProperty* property )
{
	ImGui::ColorEdit3(WStringToUTF8(property->name).c_str(), glm::value_ptr(*static_cast<glm::vec3*>(property->value)), ImGuiColorEditFlags_DisplayHSV | ImGuiColorEditFlags_HDR);
}


void DisplayColorRGBA(const ComponentProperty* property)
{
	ImGui::ColorEdit4(WStringToUTF8(property->name).c_str(), glm::value_ptr(*static_cast<glm::vec4*>(property->value)));
}


void DisplayColorRGBA_HDR( const ComponentProperty* property )
{
	ImGui::ColorEdit4(WStringToUTF8(property->name).c_str(), glm::value_ptr(*static_cast<glm::vec4*>(property->value)), ImGuiColorEditFlags_HDR);
}


void DisplayColorHSVA( const ComponentProperty* property )
{
	ImGui::ColorEdit4(WStringToUTF8(property->name).c_str(), glm::value_ptr(*static_cast<glm::vec4*>(property->value)), ImGuiColorEditFlags_DisplayHSV);
}


void DisplayColorHSVA_HDR( const ComponentProperty* property )
{
	ImGui::ColorEdit4(WStringToUTF8(property->name).c_str(), glm::value_ptr(*static_cast<glm::vec4*>(property->value)), ImGuiColorEditFlags_DisplayHSV | ImGuiColorEditFlags_HDR);
}

}
