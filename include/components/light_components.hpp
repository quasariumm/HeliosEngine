#pragma once

#include "components/basic_components.hpp"


namespace Helios::Components
{
struct DirectionalLight final : BaseComponent
{
	template <typename Archive>
	void serialise( Archive& archive ) { archive(CEREAL_NVP(color), CEREAL_NVP(intensity), CEREAL_NVP(direction)); }


	void Inspector() override;

	glm::vec3 color;
	float     intensity;
	glm::vec3 direction;
	float     padding;
};


struct PointLight final : BaseComponent
{
	template <typename Archive>
	void serialise( Archive& archive ) { archive(CEREAL_NVP(color), CEREAL_NVP(intensity), CEREAL_NVP(position)); }


	void Inspector() override;

	glm::vec3 color;
	float     intensity;
	glm::vec3 position;
};


struct SpotLight final : BaseComponent
{
	template <typename Archive>
	void serialise( Archive& archive )
	{
		archive(CEREAL_NVP(color), CEREAL_NVP(intensity), CEREAL_NVP(position), CEREAL_NVP(innerCutoff),
		        CEREAL_NVP(direction), CEREAL_NVP(outerCutoff));
	}


	void Inspector() override;

	glm::vec3 color;
	float     intensity;
	glm::vec3 position;
	float     innerCutoff;
	glm::vec3 direction;
	float     outerCutoff;
};
}
