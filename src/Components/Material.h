#pragma once

#include "Component.h"
#include "Core/IconDefines.h"

namespace Engine
{

static void DisplayMaterialProperties(void* data);
static void DisplayMicrofacetModel(void* data);

class Material final : public Component
{
public:
	Material() : Component(this, STR_TO_WSTR(ICON_PALETTE_SWATCH_VARIANT) + L" Material")
	{
		AssignProperty(L"Properties", &m_properties, &DisplayMaterialProperties);
		AssignProperty(L"MicrofacetModel", &m_microfacetModel, &DisplayMicrofacetModel);

		AssignProperty(L"Diffuse Color", &m_diffuseColor);
		AssignProperty(L"SpecularColor", &m_specularColor);
		AssignProperty(L"Specularity", &m_specularity);
		AssignProperty(L"Shininess", &m_shininess);
		AssignProperty(L"Glossiness", &m_glossiness);

		AssignProperty(L"Emission Color", &m_emissionColor);
		AssignProperty(L"EmissionStrength", &m_emissionStrength);

		AssignProperty(L"Refractivity", &m_refractivity);
		AssignProperty(L"RefractionCoefficient", &m_refractionCoefficient);

		AssignProperty(L"PBR Roughness", &m_PBR_Roughness);
		AssignProperty(L"PBR Metallic", &m_PBR_Metallic);
		AssignProperty(L"PBR Reflectance", &m_PBR_Reflectance);
	}

	void Init() override
	{
		materials.push_back(this);
	}

	struct MaterialProperties
	{
		uint8_t reflection : 1 = 0u;
		uint8_t microfacet : 1 = 0u;
		uint8_t transmission : 1 = 0u;
		uint8_t diffuse : 1 = 0u;
		uint8_t glossy : 1 = 0u;
		uint8_t specular : 1 = 0u;
	} m_properties;

	struct MicrofacetModel
	{
		uint8_t beckmann: 1 = 0u;
		uint8_t ggx_iso: 1 = 0u;
		uint8_t ggx_aniso: 1 = 0u;
		uint8_t blinnphong: 1 = 0u;
		uint8_t selector : 4 = 0u; /* for use with Dear ImGUI */
	} m_microfacetModel;

	vec3f m_diffuseColor = {};
	vec3f m_specularColor = {};
	float m_specularity = 0.f;
	float m_shininess = 0.f;
	float m_glossiness = 0.f;

	vec3f m_emissionColor = {};
	float m_emissionStrength = 0.f;

	float m_refractivity = 0.f;
	float m_refractionCoefficient = 1.f;

	float m_PBR_Roughness = 0.f;
	float m_PBR_Metallic = 0.f;
	float m_PBR_Reflectance = 0.f;

};

static void DisplayMaterialProperties(void* data)
{
	auto* mat = (Material::MaterialProperties*)data;

	// TODO: Make the material idx display sane
	const int idx = std::distance(materials.begin(), std::ranges::find(materials, data));
	ImGui::Text("Material idx: %i", idx);
	ImGui::Separator();

	bool reflection = false;
	ImGui::Checkbox("Rays reflect", &reflection);
	mat->reflection = reflection ? 1 : 0;

	bool microfacet = false;
	ImGui::Checkbox("Use microfacets", &microfacet);
	mat->microfacet = microfacet ? 1 : 0;

	bool transmission = false;
	ImGui::Checkbox("Transmission", &transmission);
	mat->transmission = transmission ? 1 : 0;

	bool diffuse = false;
	ImGui::Checkbox("Diffuse", &diffuse);
	mat->diffuse = diffuse ? 1 : 0;

	bool glossy = false;
	ImGui::Checkbox("Glossy", &glossy);
	mat->glossy = glossy ? 1 : 0;

	bool specular = false;
	ImGui::Checkbox("Specular", &specular);
	mat->transmission = specular ? 1 : 0;
}

static void DisplayMicrofacetModel(void* data)
{
	auto* model = (Material::MicrofacetModel*)data;
	ImGui::Text("Microfacet model:");
	if (ImGui::Selectable("Beckmann", model->selector == 0))
		model->selector = 0, model->beckmann = 1;
	else model->beckmann = 0;

	if (ImGui::Selectable("Isotropic GGX", model->selector == 1))
		model->selector = 1, model->ggx_iso = 1;
	else model->ggx_iso = 0;

	if (ImGui::Selectable("Anisotropic GGX", model->selector == 2))
		model->selector = 2, model->ggx_aniso = 1;
	else model->ggx_aniso = 0;

	if (ImGui::Selectable("Blinn-Phong", model->selector == 3))
		model->selector = 3, model->blinnphong = 1;
	else model->blinnphong = 0;
}

REGISTER_COMPONENT(Material);

static bool defaultMaterial = []()
{
	materials.push_back( new Material(
		Material::MaterialProperties(1, 0, 1, 1, 1, 1),
		Material::MicrofacetModel(1, 0, 0 ,0),
		vec3f(0.9f, 0.6f, 0.3f),
		vec3f(1.f),
		0.f, 0.f, 0.f,
		vec3f(0.f), 0.f,
		0.f, 1.f,
		0.3f, 0.3f, 0.3f
	));
	return true;
}();

}