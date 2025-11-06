#include "components/render_components.hpp"

using namespace Helios::Components;


void Sphere::Inspector()
{
	ImGui::InputInt("Material Index", &materialIdx);
}


void Material::Inspector()
{
	bool reflection = properties.reflection;
	if (ImGui::Checkbox("Rays reflect", &reflection))
		properties.reflection = reflection ? 1 : 0;

	bool microfacet = properties.microfacet;
	if (ImGui::Checkbox("Use microfacets", &microfacet))
		properties.microfacet = microfacet ? 1 : 0;

	bool transmission = properties.transmission;
	if (ImGui::Checkbox("Transmission", &transmission))
		properties.transmission = transmission ? 1 : 0;

	bool diffuse = properties.diffuse;
	if (ImGui::Checkbox("Diffuse", &diffuse))
		properties.diffuse = diffuse ? 1 : 0;

	bool glossy = properties.glossy;
	if (ImGui::Checkbox("Glossy", &glossy))
		properties.glossy = glossy ? 1 : 0;

	bool specular = properties.specular;
	if (ImGui::Checkbox("Specular", &specular))
		properties.specular = specular ? 1 : 0;

	ImGui::Separator();
	ImGui::Text("Microfacet model:");
	if (ImGui::Selectable("Beckmann", properties.microfacetSelector == 0))
		properties.microfacetSelector = 0;

	if (ImGui::Selectable("Isotropic GGX", properties.microfacetSelector == 1))
		properties.microfacetSelector = 1;

	if (ImGui::Selectable("Anisotropic GGX", properties.microfacetSelector == 2))
		properties.microfacetSelector = 2;

	if (ImGui::Selectable("Blinn-Phong", properties.microfacetSelector == 3))
		properties.microfacetSelector = 3;

	properties.beckmann   = (properties.microfacetSelector == 0) ? 1 : 0;
	properties.ggx_iso    = (properties.microfacetSelector == 1) ? 1 : 0;
	properties.ggx_aniso  = (properties.microfacetSelector == 2) ? 1 : 0;
	properties.blinnphong = (properties.microfacetSelector == 3) ? 1 : 0;

	if (properties.microfacetSelector == 2)
	{
		ImGui::Separator();
		ImGui::InputFloat("Alpha X", &properties.alphaX, 0.001f);
		ImGui::InputFloat("Alpha Y", &properties.alphaY, 0.001f);
	}
	ImGui::Separator();

	ImGui::ColorEdit3("Diffuse Color", glm::value_ptr(diffuseColor),
	                  ImGuiColorEditFlags_InputHSV | ImGuiColorEditFlags_DisplayHSV);
	ImGui::ColorEdit3("Specular Color", glm::value_ptr(specularColor),
	                  ImGuiColorEditFlags_InputHSV | ImGuiColorEditFlags_DisplayHSV);

	ImGui::SliderFloat("Specularity", &specularity, 0.0f, 1.0f);
	ImGui::DragFloat("Shininess", &shininess, 0.01f);

	ImGui::ColorEdit3("Emission Color", glm::value_ptr(emissionColor),
	                  ImGuiColorEditFlags_InputHSV | ImGuiColorEditFlags_DisplayHSV);
	ImGui::DragFloat("Emission Strength", &emissionStrength, 0.01f, 0.f, 1e6f);

	ImGui::SliderFloat("Refractivity", &refractivity, 0.0f, 1.0f);
	ImGui::DragFloat("Refraction Coefficient", &refractionCoefficient, 0.01f, 0.f, 10.f);

	ImGui::DragFloat("Absorption", &absorption, 0.01f, 0.f, 100.f);

	ImGui::SliderFloat("PBR Roughness", &PBR_Roughness, 0.0f, 1.0f);
	ImGui::SliderFloat("PBR Metallic", &PBR_Metallic, 0.0f, 1.0f);
}
