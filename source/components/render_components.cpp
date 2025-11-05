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
	if (ImGui::Selectable("Beckmann", microfacetModel.selector == 0))
		microfacetModel.selector = 0;

	if (ImGui::Selectable("Isotropic GGX", microfacetModel.selector == 1))
		microfacetModel.selector = 1;

	if (ImGui::Selectable("Anisotropic GGX", microfacetModel.selector == 2))
		microfacetModel.selector = 2;

	if (ImGui::Selectable("Blinn-Phong", microfacetModel.selector == 3))
		microfacetModel.selector = 3;

	microfacetModel.beckmann   = (microfacetModel.selector == 0) ? 1 : 0;
	microfacetModel.ggx_iso    = (microfacetModel.selector == 1) ? 1 : 0;
	microfacetModel.ggx_aniso  = (microfacetModel.selector == 2) ? 1 : 0;
	microfacetModel.blinnphong = (microfacetModel.selector == 3) ? 1 : 0;

	if (microfacetModel.selector == 2)
	{
		ImGui::Separator();
		ImGui::InputFloat("Alpha X", &microfacetModel.alphaX);
		ImGui::InputFloat("Alpha Y", &microfacetModel.alphaY);
	}
	ImGui::Separator();

	ImGui::ColorEdit3("Diffuse Color", glm::value_ptr(diffuseColor),
	                  ImGuiColorEditFlags_InputHSV | ImGuiColorEditFlags_DisplayHSV);
	ImGui::ColorEdit3("Specular Color", glm::value_ptr(specularColor),
	                  ImGuiColorEditFlags_InputHSV | ImGuiColorEditFlags_DisplayHSV);

	ImGui::SliderFloat("Specularity", &specularity, 0.0f, 1.0f);
	ImGui::DragFloat("Shininess", &shininess, 0.01f);
	ImGui::DragFloat("Glossiness", &glossiness, 0.01f);

	ImGui::ColorEdit3("Emission Color", glm::value_ptr(emissionColor),
	                  ImGuiColorEditFlags_InputHSV | ImGuiColorEditFlags_DisplayHSV);
	ImGui::DragFloat("Emission Strength", &emissionStrength);

	ImGui::SliderFloat("Refractivity", &refractivity, 0.0f, 1.0f);
	ImGui::DragFloat("Refraction Coefficient", &refractionCoefficient);

	ImGui::DragFloat("Absorption", &absorption, 0.01f);

	ImGui::SliderFloat("PBR Roughness", &PBR_Roughness, 0.0f, 1.0f);
	ImGui::SliderFloat("PBR Metallic", &PBR_Metallic, 0.0f, 1.0f);
}
