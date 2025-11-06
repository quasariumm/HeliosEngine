#pragma once
#include "components/basic_components.hpp"
#include "tools/icon_defines.hpp"


namespace Helios::Components
{
#define COMPONENT_FIELD_DECL(type, name, value) type name{value};
#define COMPONENT_FIELD_INIT(type, name, value) .name = name,

// A render component with a transform and entity on the GPU
#define RENDER_COMPONENT(name, fields_macro) \
	struct name##GPU final { \
		fields_macro(COMPONENT_FIELD_DECL) \
	}; \
	struct name final : BaseComponent { \
		name() = default; \
		name##GPU MakeGPU(const glm::mat4& = glm::mat4(1.f), entt::entity = entt::null) const { \
			return name##GPU{ fields_macro(COMPONENT_FIELD_INIT) }; \
		} \
		fields_macro(COMPONENT_FIELD_DECL) \
		bool dirty = false;

// A render component with a padding on the gpu
#define RENDER_COMPONENT_PADDED(name, fields_macro, gpu_padding) \
	struct name##GPU final { \
		fields_macro(COMPONENT_FIELD_DECL) \
		uint8_t padding[gpu_padding]; \
	}; \
	struct name final : BaseComponent { \
		name() = default; \
		name##GPU MakeGPU(const glm::mat4& = glm::mat4(1.f), entt::entity = entt::null) const { \
			return name##GPU{ fields_macro(COMPONENT_FIELD_INIT) }; \
		} \
		fields_macro(COMPONENT_FIELD_DECL) \
		bool dirty = false;

// A render component with a transform and entity on the GPU
#define RENDER_COMPONENT_TRANSFORM_ENTITY_PADDED(name, fields_macro, gpu_padding) \
	struct name##GPU final { \
		glm::mat4 transform{1.f}; \
		fields_macro(COMPONENT_FIELD_DECL) \
		entt::entity entity{entt::null}; \
		uint8_t padding[gpu_padding]; \
	}; \
	struct name final : BaseComponent { \
		name() = default; \
		name##GPU MakeGPU(const glm::mat4& transform, entt::entity entity) const { \
			return name##GPU{ .transform = transform, fields_macro(COMPONENT_FIELD_INIT) .entity = entity }; \
		} \
		fields_macro(COMPONENT_FIELD_DECL) \
		bool dirty = false;

/****************************************************************************************************************
 *											Render Components													*
 ****************************************************************************************************************/

// Define your fields once
#define SPHERE_FIELDS(F) \
	F(int32_t, materialIdx, 0)

// Usage
RENDER_COMPONENT_TRANSFORM_ENTITY_PADDED(Sphere, SPHERE_FIELDS, 8)

	//
	// template <typename Archive>
	// void serialize( Archive& archive ) { archive(CEREAL_NVP(radius), CEREAL_NVP(position), CEREAL_NVP(materialIdx)); }


	void Inspector() override;
};


REGISTER_COMPONENT(Sphere, ICON_SPHERE" Sphere", ADDABLE | INSPECTABLE)


struct alignas(4) MaterialProperties
{
	uint8_t reflection   : 1 = 1u;
	uint8_t microfacet   : 1 = 0u;
	uint8_t transmission : 1 = 1u;
	uint8_t diffuse      : 1 = 1u;
	uint8_t glossy       : 1 = 1u;
	uint8_t specular     : 1 = 1u;
	uint8_t bitPadding   : 2 = 0u;
	// Microfacet model
	uint8_t beckmann           : 1 = 1u;
	uint8_t ggx_iso            : 1 = 0u;
	uint8_t ggx_aniso          : 1 = 0u;
	uint8_t blinnphong         : 1 = 0u;
	uint8_t microfacetSelector : 4 = 0u; /* for use with Dear ImGUI */

	char padding[2];

	float alphaX = 0.f; /* For use with anisotropic GGX */
	float alphaY = 0.f; /* For use with anisotropic GGX */

private:

	// friend class cereal::access;

	template <typename Archive>
	void save( Archive& archive ) const;

	template <typename Archive>
	void load( Archive& archive );
};


#define MATERIAL_FIELDS(F) \
	F(MaterialProperties, properties, MaterialProperties{}) \
	F(float, specularity, 0.f) \
	F(glm::vec3, diffuseColor, 0.f) \
	F(float, shininess, 0.f) \
	F(glm::vec3, specularColor, 0.f) \
	F(float, emissionStrength, 0.f) \
	F(glm::vec3, emissionColor, 0.f) \
	F(float, refractivity, 0.f) \
	F(float, refractionCoefficient, 1.f) \
	F(float, absorption, 0.1f) \
	F(float, PBR_Roughness, 0.f) \
	F(float, PBR_Metallic, 0.f)

RENDER_COMPONENT(Material, MATERIAL_FIELDS)

	void Inspector() override;
};


REGISTER_COMPONENT(Material, ICON_PALETTE_SWATCH_VARIANT" Material", ADDABLE | INSPECTABLE);


// template <typename Archive>
// void MaterialProperties::save( Archive& archive ) const
// {
// 	uint8_t properties = reflection | (microfacet << 1) | (transmission << 2) | (diffuse << 3) | (glossy << 4) | (
// 		                     specular << 5);
// 	uint8_t microfacetModel = beckmann | (ggx_iso << 1) | (ggx_aniso << 2) | (blinnphong << 3) | (
// 		                          microfacetSelector << 4);
// 	archive(CEREAL_NVP(properties), CEREAL_NVP(microfacetModel), CEREAL_NVP(alphaX), CEREAL_NVP(alphaY));
// }


// template <typename Archive>
// void MaterialProperties::load( Archive& archive )
// {
// 	uint8_t properties;
// 	uint8_t microfacetModel;
// 	archive(properties, microfacetModel, alphaX, alphaY);
// 	reflection   = properties & 0x1;
// 	microfacet   = (properties & 0x2) >> 1;
// 	transmission = (properties & 0x4) >> 2;
// 	diffuse      = (properties & 0x8) >> 3;
// 	glossy       = (properties & 0x10) >> 4;
// 	specular     = (properties & 0x20) >> 5;
//
// 	beckmann           = microfacetModel & 0x1;
// 	ggx_iso            = (microfacetModel & 0x2) >> 1;
// 	ggx_aniso          = (microfacetModel & 0x4) >> 2;
// 	blinnphong         = (microfacetModel & 0x8) >> 3;
// 	microfacetSelector = (microfacetModel & 0xF0) >> 4;
// }


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
