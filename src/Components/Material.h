#pragma once

#include "Component.h"

namespace Engine
{

class Material;

class MaterialRegister
{
public:
	static ENGINE_API MaterialRegister& Instance()
	{
		static MaterialRegister instance;
		return instance;
	}

	void RegisterMaterial(Material* mat) { if (mat != nullptr) m_materials.push_back(mat); }

	void DeregisterMaterial(Material* mat) { m_materials.erase(std::ranges::find(m_materials, mat)); }

	[[nodiscard]]
	Material* GetMaterial(const int idx) const
	{
		// If requested material is invalid, use the default
		if (idx < 0 || idx >= m_materials.size() || m_materials[idx] == nullptr)
		{
			if (m_defaultMaterial == nullptr)
				throw std::invalid_argument("Default material has not been created");
			return m_defaultMaterial;
		}

		return m_materials[idx];
	}

	[[nodiscard]]
	int GetNumMaterials() const { return static_cast<int>(m_materials.size()); }

	[[nodiscard]]
	std::vector<Material*>& GetMaterials() { return m_materials; }

	void SetDefaultMaterial(Material* mat) { m_defaultMaterial = mat; }

private:
	std::vector<Material*> m_materials;

	Material* m_defaultMaterial = nullptr;
};

static void DisplayMaterialProperties( const ComponentProperty* property);
static void DisplayMicrofacetModel( const ComponentProperty* property);

class Material final : public Component
{
public:
	Material() : Component(this)
	{
		AssignProperty(L"Properties", &m_properties, &DisplayMaterialProperties);
		AssignProperty(L"MicrofacetModel", &m_microfacetModel, &DisplayMicrofacetModel);

		AssignProperty(L"Diffuse Color", &m_diffuseColor, &DisplayColorRGB);
		AssignProperty(L"SpecularColor", &m_specularColor, &DisplayColorRGB);
		AssignProperty(L"Specularity", &m_specularity);
		AssignProperty(L"Shininess", &m_shininess);
		AssignProperty(L"Glossiness", &m_glossiness);

		AssignProperty(L"Emission Color", &m_emissionColor, &DisplayColorRGB);
		AssignProperty(L"EmissionStrength", &m_emissionStrength);

		AssignProperty(L"Refractivity", &m_refractivity);
		AssignProperty(L"RefractionCoefficient", &m_refractionCoefficient);

		AssignProperty(L"PBR Roughness", &m_PBR_Roughness);
		AssignProperty(L"PBR Metallic", &m_PBR_Metallic);
		AssignProperty(L"PBR Reflectance", &m_PBR_Reflectance);
	}

	~Material() override
	{
		MaterialRegister::Instance().DeregisterMaterial(this);
	}

	static ENGINE_API void SetupDefaultMaterial()
	{
		static Material material;
		material.m_properties = { 1, 0, 1, 1, 1, 1 };
		material.m_microfacetModel = { 1, 0, 0, 0 };
		material.m_diffuseColor = vec3f(0.9f, 0.6f, 0.3f);
		material.m_specularColor = vec3f(1.f, 1.f, 1.f);
		material.m_refractionCoefficient = 1.f;
		MaterialRegister::Instance().RegisterMaterial(&material);
	}

	void Init() override
	{
		MaterialRegister::Instance().RegisterMaterial(this);
	}

	struct MaterialProperties
	{
		uint8_t reflection : 1 = 1u;
		uint8_t microfacet : 1 = 0u;
		uint8_t transmission : 1 = 1u;
		uint8_t diffuse : 1 = 1u;
		uint8_t glossy : 1 = 1u;
		uint8_t specular : 1 = 1u;
	} m_properties;

	struct MicrofacetModel
	{
		uint8_t beckmann: 1 = 1u;
		uint8_t ggx_iso: 1 = 0u;
		uint8_t ggx_aniso: 1 = 0u;
		uint8_t blinnphong: 1 = 0u;
		uint8_t selector : 4 = 0u; /* for use with Dear ImGUI */
		char padding[3]{0};
		float alphaX = 0.f; /* For use with anisotropic GGX */
		float alphaY = 0.f; /* For use with anisotropic GGX */
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

static void DisplayMaterialProperties(const ComponentProperty* property)
{
	auto* mat = static_cast<Material::MaterialProperties*>(property->value);

	static Material offsetMaterial = {};
	static size_t propertiesOffset = (size_t)&offsetMaterial.m_properties - (size_t)&offsetMaterial;

	std::vector<Material*> materialList = MaterialRegister::Instance().GetMaterials();
	const int idx = std::distance(materialList.begin(), std::ranges::find(materialList, (Material*)((intptr_t)mat - (intptr_t)propertiesOffset)));
	ImGui::Text("Material idx: %i", idx);
	if (ImGui::Button("Apply changes"))
	{
		ObjectRenderer::Instance().UpdateMaterialSSBO();
	}
	ImGui::Separator();

	bool reflection = mat->reflection;
	if (ImGui::Checkbox("Rays reflect", &reflection))
		mat->reflection = reflection ? 1 : 0;

	bool microfacet = mat->microfacet;
	if (ImGui::Checkbox("Use microfacets", &microfacet))
		mat->microfacet = microfacet ? 1 : 0;

	bool transmission = mat->transmission;
	if (ImGui::Checkbox("Transmission", &transmission))
		mat->transmission = transmission ? 1 : 0;

	bool diffuse = mat->diffuse;
	if (ImGui::Checkbox("Diffuse", &diffuse))
		mat->diffuse = diffuse ? 1 : 0;

	bool glossy = mat->glossy;
	if (ImGui::Checkbox("Glossy", &glossy))
		mat->glossy = glossy ? 1 : 0;

	bool specular = mat->specular;
	if (ImGui::Checkbox("Specular", &specular))
		mat->specular = specular ? 1 : 0;
}

static void DisplayMicrofacetModel(const ComponentProperty* property)
{
	auto* model = static_cast<Material::MicrofacetModel*>(property->value);
	ImGui::Text("Microfacet model:");
	if (ImGui::Selectable("Beckmann", model->selector == 0))
		model->selector = 0;

	if (ImGui::Selectable("Isotropic GGX", model->selector == 1))
		model->selector = 1;

	if (ImGui::Selectable("Anisotropic GGX", model->selector == 2))
		model->selector = 2;

	if (ImGui::Selectable("Blinn-Phong", model->selector == 3))
		model->selector = 3;

	model->beckmann = (model->selector == 0) ? 1 : 0;
	model->ggx_iso = (model->selector == 1) ? 1 : 0;
	model->ggx_aniso = (model->selector == 2) ? 1 : 0;
	model->blinnphong = (model->selector == 3) ? 1 : 0;

	if (model->selector == 2)
	{
		ImGui::Separator();
		ImGui::InputFloat("Alpha X", &model->alphaX);
		ImGui::InputFloat("Alpha Y", &model->alphaY);
	}
	ImGui::Separator();
}

REGISTER_COMPONENT(Material, STR_TO_WSTR(ICON_PALETTE_SWATCH_VARIANT) + L" Material");
}
