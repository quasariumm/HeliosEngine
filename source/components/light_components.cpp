#include "components/light_components.hpp"

using namespace Engine::Components;


void DirectionalLight::Inspector()
{
	if (ImGui::DragFloat3("Direction", glm::value_ptr(direction), 0.01f))
		direction = glm::normalize(direction);

	ImGui::ColorEdit3("Color", glm::value_ptr(color), ImGuiColorEditFlags_InputHSV | ImGuiColorEditFlags_DisplayHSV);
	ImGui::DragFloat("Intensity", &intensity, 0.01f);
}


void PointLight::Inspector()
{
	ImGui::DragFloat3("Position", glm::value_ptr(position), 0.01f);

	ImGui::ColorEdit3("Color", glm::value_ptr(color), ImGuiColorEditFlags_InputHSV | ImGuiColorEditFlags_DisplayHSV);
	ImGui::DragFloat("Intensity", &intensity, 0.01f);
}
