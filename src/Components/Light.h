#pragma once

#include "Component.h"

namespace Engine
{

enum class LightType : uint8_t
{
	POINT = 0,
	DIRECTIONAL,
	SIMPLE_SPOT,
	IES_SPOT /* TODO */,
	MAX
};

class Light final : public Component
{
public:
	Light() : Component(this)
	{
		AssignProperty(L"Type", &lightType);

		AssignProperty(L"Color", &color);
		AssignProperty(L"Position", &position);
		AssignProperty(L"Intensity", &intensity);

		AssignProperty(L"Direction", &direction);

		AssignProperty(L"Inner cutoff", &innerCutOff);
		AssignProperty(L"Outer cutoff", &outerCutOff);
	}

	void Init() override
	{
		lights.push_back(this);
	}

	void DisplayProperties() override
	{
		int _lightType = (int)lightType;
		if (ImGui::Combo("Light type", &_lightType, "Point\0Directional\0Simple Spot\0IES Spot\0\0"))
			lightType = (LightType)_lightType;

		ImGui::ColorEdit3("Color", glm::value_ptr(color));
		if (lightType != LightType::DIRECTIONAL)
			ImGui::DragFloat3("Position", glm::value_ptr(position));
		ImGui::DragFloat("Intensity", &intensity, 0.001f, 0.f);

		switch (lightType)
		{
		case LightType::SIMPLE_SPOT:
			ImGui::SliderFloat("Inner cutoff", &innerCutOff, 0.f, 1.f);
			ImGui::SliderFloat("Outer cutoff", &outerCutOff, 0.f, 1.f);
			[[fallthrough]];
		case LightType::DIRECTIONAL:
		case LightType::IES_SPOT:
			if (ImGui::DragFloat3("Direction", glm::value_ptr(direction), 0.01f))
				direction = glm::normalize(direction);
			break;
		default:
			break;
		}
	}

	LightType lightType = LightType::POINT;

	glm::vec3 color = glm::vec3(0.f);
	glm::vec3 position = glm::vec3(0.f);
	float intensity = 0.f;

	glm::vec3 direction = glm::vec3(0.f);

	float innerCutOff = 0.f;
	float outerCutOff = 0.f;

	// IES
	// Non-user-editable. Made with the LoadIES function
	glm::vec3 iesDimensions = glm::vec3(0.f);
	float iesMaxCD = 0.f;
	float iesRowStepSize = 0.f;
	float iesColStepSize = 0.f;
	int32_t iesRows = 0;
	int32_t iesCols = 0;
	float* iesData = nullptr;
};

REGISTER_COMPONENT(Light, STR_TO_WSTR(ICON_LIGHTBULB) + L" Light");

}