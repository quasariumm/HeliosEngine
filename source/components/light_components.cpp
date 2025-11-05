#include "components/light_components.hpp"

using namespace Helios::Components;


void DirectionalLight::Inspector()
{
	ImGui::ColorEdit3("Color", glm::value_ptr(color), ImGuiColorEditFlags_InputHSV | ImGuiColorEditFlags_DisplayHSV);
	ImGui::DragFloat("Intensity", &intensity, 0.01f);

	if (ImGui::DragFloat3("Direction", glm::value_ptr(direction), 0.01f))
		direction = glm::normalize(direction);
}


void PointLight::Inspector()
{
	ImGui::ColorEdit3("Color", glm::value_ptr(color), ImGuiColorEditFlags_InputHSV | ImGuiColorEditFlags_DisplayHSV);
	ImGui::DragFloat("Intensity", &intensity, 0.01f);

	ImGui::DragFloat3("Position", glm::value_ptr(position), 0.01f);
}


void SpotLight::Inspector()
{
	ImGui::ColorEdit3("Color", glm::value_ptr(color), ImGuiColorEditFlags_InputHSV | ImGuiColorEditFlags_DisplayHSV);
	ImGui::DragFloat("Intensity", &intensity, 0.01f);

	ImGui::DragFloat3("Position", glm::value_ptr(position), 0.01f);
	ImGui::DragFloat3("Direction", glm::value_ptr(direction), 0.01f);

	ImGui::SliderFloat("Inner cutoff", &innerCutoff, 0.f, 1.f);
	ImGui::SliderFloat("Outer cutoff", &outerCutoff, 0.f, 1.f);
}
