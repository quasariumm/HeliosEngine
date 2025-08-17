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
		AssignProperty(L"Ground color", &GroundColor);
		AssignProperty(L"Horizon color", &HorizonColor);
		AssignProperty(L"Zenith color", &ZenithColor);
		AssignProperty(L"Sun direction", &SunDirection);
		AssignProperty(L"Sun focus", &SunDirection);
		AssignProperty(L"Sun intensity", &SunIntensity);
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
			ImGui::Text(path.string().c_str());
			// Set shader values
			m_texture.Use(31);
			shader->SetInt("SkyboxTexture", 31);
		}
		else
		{
			Component::DisplayProperties();
			SunDirection = Normalise(SunDirection);
			// Set shader values
			shader->SetVec3("GroundColor", GroundColor);
			shader->SetVec3("HorizonColor", HorizonColor);
			shader->SetVec3("ZenithColor", ZenithColor);
			shader->SetVec3("SunDirection", SunDirection);
			shader->SetFloat("SunFocus", SunFocus);
			shader->SetFloat("SunIntensity", SunIntensity);
		}
	}

	vec3f GroundColor  = vec3f(0.5f,  0.5f,  0.5f);
	vec3f HorizonColor = vec3f(0.78f, 0.87f, 1.0f);
	vec3f ZenithColor  = vec3f(0.68f, 0.84f, 1.0f);

	vec3f SunDirection	= Normalise(vec3f(1.f, -1.f, 0.f));
	float SunFocus    	= 150.f;
	float SunIntensity	= 3.f;

private:

	bool m_useTexture = false;

	std::wstring m_texturePath;

	GL46_Texture2D m_texture;

};

REGISTER_COMPONENT(Skybox, STR_TO_WSTR(ICON_WEATHER_PARTLY_CLOUDY) + L" Skybox");

}
