#pragma once
#include "components/basic_components.hpp"
#include "tools/icon_defines.hpp"


namespace Helios::Components
{
#define COMPONENT_FIELD_DECL(type, name, value) type name{value};
#define COMPONENT_FIELD_INIT(type, name, value) .name = name,

#define RENDER_COMPONENT(name, fields_macro, gpu_padding) \
	struct name##GPU final { \
		fields_macro(COMPONENT_FIELD_DECL) \
		entt::entity entity{entt::null}; \
		uint8_t padding[gpu_padding]; \
	}; \
	struct name final : BaseComponent { \
		name() = default; \
		name##GPU MakeGPU(entt::entity entity) const { \
			return name##GPU{ fields_macro(COMPONENT_FIELD_INIT) .entity = entity }; \
		} \
		fields_macro(COMPONENT_FIELD_DECL) \
		bool dirty = false;
// Define your fields once
#define SPHERE_FIELDS(F) \
	F(glm::vec3, position, 0.f) \
	F(float, radius, 1.f) \
	F(int32_t, materialIdx, 0)

// Usage
RENDER_COMPONENT(Sphere, SPHERE_FIELDS, 8)


	template <typename Archive>
	void serialize( Archive& archive ) { archive(CEREAL_NVP(radius), CEREAL_NVP(position), CEREAL_NVP(materialIdx)); }


	void Inspector() override;
};


REGISTER_COMPONENT(Sphere, ICON_SPHERE" Sphere", ADDABLE | INSPECTABLE)


struct Material final : BaseComponent
{
	static Material& GetDefaultMaterial()
	{
		static Material material;
		material.properties            = {1, 0, 1, 1, 1, 1};
		material.microfacetModel       = {1, 0, 0, 0};
		material.diffuseColor          = glm::vec3(0.9f, 0.6f, 0.3f);
		material.specularColor         = glm::vec3(1.f, 1.f, 1.f);
		material.refractionCoefficient = 1.f;
		return material;
	}


	template <typename Archive>
	void serialize( Archive& archive )
	{
		archive(CEREAL_NVP(properties), CEREAL_NVP(microfacetModel), CEREAL_NVP(diffuseColor),
		        CEREAL_NVP(specularColor), CEREAL_NVP(specularity), CEREAL_NVP(shininess), CEREAL_NVP(glossiness),
		        CEREAL_NVP(emissionColor), CEREAL_NVP(emissionStrength), CEREAL_NVP(refractivity),
		        CEREAL_NVP(refractionCoefficient), CEREAL_NVP(absorption), CEREAL_NVP(PBR_Roughness),
		        CEREAL_NVP(PBR_Metallic));
	}


	void Inspector() override;


	struct alignas(4) MaterialProperties
	{
		uint8_t reflection   : 1 = 1u;
		uint8_t microfacet   : 1 = 0u;
		uint8_t transmission : 1 = 1u;
		uint8_t diffuse      : 1 = 1u;
		uint8_t glossy       : 1 = 1u;
		uint8_t specular     : 1 = 1u;
		uint8_t bitPadding   : 2 = 0u;
		char    padding[3];

	private:

		friend class cereal::access;

		template <typename Archive>
		void save( Archive& archive ) const;

		template <typename Archive>
		void load( Archive& archive );
	} properties;


	struct MicrofacetModel
	{
		uint8_t beckmann   : 1 = 1u;
		uint8_t ggx_iso    : 1 = 0u;
		uint8_t ggx_aniso  : 1 = 0u;
		uint8_t blinnphong : 1 = 0u;
		uint8_t selector   : 4 = 0u; /* for use with Dear ImGUI */
		char    padding[3];
		float   alphaX = 0.f; /* For use with anisotropic GGX */
		float   alphaY = 0.f; /* For use with anisotropic GGX */

	private:

		friend class cereal::access;

		template <typename Archive>
		void save( Archive& archive ) const;

		template <typename Archive>
		void load( Archive& archive );
	} microfacetModel;


	glm::vec3 diffuseColor  = {};
	glm::vec3 specularColor = {};
	float     specularity   = 0.f;
	float     shininess     = 0.f;
	float     glossiness    = 0.f;

	glm::vec3 emissionColor    = {};
	float     emissionStrength = 0.f;

	float refractivity          = 0.f;
	float refractionCoefficient = 1.f;
	float absorption            = 0.1f;

	float PBR_Roughness = 0.f;
	float PBR_Metallic  = 0.f;
};


REGISTER_COMPONENT(Material, ICON_PALETTE_SWATCH_VARIANT" Material", ADDABLE | INSPECTABLE);


template <typename Archive>
void Material::MaterialProperties::save( Archive& archive ) const
{
	uint8_t data = reflection | (microfacet << 1) | (transmission << 2) | (diffuse << 3) | (glossy << 4) | (
		               specular << 5);
	archive(CEREAL_NVP(data));
}


template <typename Archive>
void Material::MaterialProperties::load( Archive& archive )
{
	uint8_t data;
	archive(data);
	reflection   = data & 0x1;
	microfacet   = (data & 0x2) >> 1;
	transmission = (data & 0x4) >> 2;
	diffuse      = (data & 0x8) >> 3;
	glossy       = (data & 0x10) >> 4;
	specular     = (data & 0x20) >> 5;
}


template <typename Archive>
void Material::MicrofacetModel::save( Archive& archive ) const
{
	const uint8_t tmp   = beckmann | (ggx_iso << 1) | (ggx_aniso << 2) | (blinnphong << 3);
	uint8_t       model = tmp;
	if (ggx_iso)
		model = tmp | (1 << 4);
	if (ggx_aniso)
		model = tmp | (2 << 4);
	if (blinnphong)
		model = tmp | (3 << 4);
	archive(CEREAL_NVP(model), CEREAL_NVP(alphaX), CEREAL_NVP(alphaY));
}


template <typename Archive>
void Material::MicrofacetModel::load( Archive& archive )
{
	uint8_t model;
	archive(model, alphaX, alphaY);
	beckmann   = model & 0x1;
	ggx_iso    = (model & 0x2) >> 1;
	ggx_aniso  = (model & 0x4) >> 2;
	blinnphong = (model & 0x8) >> 3;
	selector   = (model & 0xF0) >> 4;
}


// TODO(Patrick): Make a renderable component pls :)
// struct RenderObject final : BaseComponent
// {
//     RenderObject() = default;
//     RenderObject(const resources::ResourceHandle<resources::Model>& resource) : model(resource) {}
//
//     resources::ResourceHandle<resources::Model> model = {};
//
//     template<typename Archive>
//     std::string save_minimal(Archive const &) const
//     {
//         return model.rawData->filePath.generic_string();
//     }
//
//     template<typename Archive>
//     void load_minimal(Archive const &, std::string const & value)
//     {
//         model = resources::LoadResource<resources::Model>(value);
//     }
// };
//
// REGISTER_COMPONENT(RenderObject, "Render Object", NONE);


// struct Camera final : BaseComponent
// {
//     float fov = 65.0f;
//     float near_plane = 0.01f;
//     float far_plane = 1000.0f;
//
//     template<typename Archive>
//     void serialize(Archive &archive) { archive(CEREAL_NVP(fov), CEREAL_NVP(near_plane), CEREAL_NVP(far_plane)); }
//
//     void Inspector() override;
// };
//
// REGISTER_COMPONENT(Camera, ICON_VIDEO" Camera", ADDABLE | INSPECTABLE);
}
