#pragma once
#include "Component.h"
#include "Backends/OpenGL46_GLFW/Graphics/GL46_ComputeShader.h"
#include "Backends/OpenGL46_GLFW/Graphics/GL46_Texture2D.h"
#include "Objects/ObjectRenderer.h"


namespace Engine
{

class Skybox final : public Component
{

public:

	Skybox() : Component(this)
		, m_texturePath(L"")
	{
		AssignProperty(L"Ground color", &GroundColor, &DisplayColorRGB);
		AssignProperty(L"Horizon color", &HorizonColor, &DisplayColorRGB);
		AssignProperty(L"Zenith color", &ZenithColor, &DisplayColorRGB);
		AssignProperty(L"Sun direction", &SunDirection);
		AssignProperty(L"Sun focus", &SunFocus);
		AssignProperty(L"Sun intensity", &SunIntensity);

		AssignProperty(L"Use texture", &m_useTexture, &DisplayNone);
		AssignProperty(L"HDR Texture Path", &m_texturePath, &DisplayNone);
	}

	void OnLoad() override
	{
		if (!m_useTexture)
			return;
		m_texture.LoadFromFile(m_texturePath, TextureFormat::RGB32F, true);
		GL46_ComputeShader* shader = ObjectRenderer::Instance().GetShader();
		shader->Use();
		// Set shader values
		m_texture.Use(31);
		shader->SetBool("UseSkyboxTexture", m_useTexture);
		shader->SetInt("SkyboxTexture", 31);
	}

	void DisplayProperties() override
	{
		ImGui::Checkbox("Use HDR texture", &m_useTexture);

		GL46_ComputeShader* shader = ObjectRenderer::Instance().GetShader();
		if (shader == nullptr)
			return;
		shader->Use();

		shader->SetBool("UseSkyboxTexture", m_useTexture);
		if (m_useTexture)
		{
			std::filesystem::path path{m_texturePath};
			if (ImGui::Button("Browse"))
			{
				if (ProjectHandler::ShowFileSelect(path, HDRTextureFilters))
				{
					m_texturePath = path.wstring();
					m_texture.LoadFromFile(path.wstring(), TextureFormat::RGB32F, true);
				}
			}
			ImGui::SameLine();
			ImGui::Text("%s", path.string().c_str());
			// Set shader values
			m_texture.Use(31);
			shader->SetInt("SkyboxTexture", 31);
		}
		else
		{
			Component::DisplayProperties();
			SunDirection = glm::normalize(SunDirection);
			// Set shader values
			shader->SetVec3("GroundColor", GroundColor);
			shader->SetVec3("HorizonColor", HorizonColor);
			shader->SetVec3("ZenithColor", ZenithColor);
			shader->SetVec3("SunDirection", SunDirection);
			shader->SetFloat("SunFocus", SunFocus);
			shader->SetFloat("SunIntensity", SunIntensity);
		}
	}

	glm::vec3 GroundColor  = glm::vec3(0.5f,  0.5f,  0.5f);
	glm::vec3 HorizonColor = glm::vec3(0.78f, 0.87f, 1.0f);
	glm::vec3 ZenithColor  = glm::vec3(0.68f, 0.84f, 1.0f);

	glm::vec3 SunDirection	= glm::normalize(glm::vec3(1.f, -1.f, 0.f));
	float SunFocus    	= 150.f;
	float SunIntensity	= 3.f;

private:

	bool m_useTexture = false;

	std::wstring m_texturePath;

	GL46_Texture2D m_texture;

};

REGISTER_COMPONENT(Skybox, STR_TO_WSTR(ICON_WEATHER_PARTLY_CLOUDY) + L" Skybox");

}
