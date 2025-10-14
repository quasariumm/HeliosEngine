#pragma once

#include "components/basic_components.hpp"

namespace Engine::Components
{

struct DirectionalLight final : BaseComponent
{
	template <typename Archive>
	void serialise( Archive & archive ) { archive(CEREAL_NVP(direction), CEREAL_NVP(color), CEREAL_NVP(intensity)); }

	void Inspector() override;

	glm::vec3 direction;
	glm::vec3 color;
	float intensity;
};


struct PointLight final : BaseComponent
{
	template <typename Archive>
	void serialise( Archive & archive ) { archive(CEREAL_NVP(direction), CEREAL_NVP(color), CEREAL_NVP(intensity)); }

	void Inspector() override;

	glm::vec3 direction;
	glm::vec3 color;
	float intensity;
};

}