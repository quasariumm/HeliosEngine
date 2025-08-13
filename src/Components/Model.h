#pragma once
#include "Component.h"
#include "imgui.h"
#include "Assets/ModelFileHandler.h"
#include "Core/Common.h"


namespace Engine
{
struct ModelData;

static void DisplayModelPath(void* data);

class Model final : public Component
{

public:

	Model() : Component(this)
	{
		AssignProperty(L"Model Path", &modelPath, DisplayModelPath);
	}

	void Init() override
	{
		ObjectRenderer::Instance().RegisterModelInstance(m_attachedObject->GetTransform(), &modelData);
	}

	void DisplayProperties() override
	{
		Component::DisplayProperties();

		// Check if the model path has changed. If so, load the model
		if (modelPath != m_cachedModelPath)
		{
			m_cachedModelPath = modelPath;
			modelData = ModelFileHandler::LoadModel(modelPath);
			ObjectRenderer::Instance().UpdateModelSSBOs();
		}

		if (modelData == nullptr) return;
		// Display the meshes and their properties
		for (MeshData& mesh : modelData->meshes)
		{
			ImGui::PushID(&mesh);
			if (ImGui::TreeNode("Mesh"))
			{
				if (ImGui::InputInt("Material Index", &mesh.materialIndex))
					ObjectRenderer::Instance().UpdateModelSSBOs();
				ImGui::TreePop();
			}
			ImGui::PopID();
		}
	}

	std::wstring modelPath;

	ModelData* modelData = nullptr;

private:

	std::wstring m_cachedModelPath;

};

inline void DisplayModelPath(void* data)
{
	auto* wstring = (std::wstring*)data;
	std::filesystem::path path{*wstring};
	ImGui::Text(path.string().c_str());
	ImGui::SameLine();
	if (ImGui::Button("Browse"))
		if (ProjectHandler::ShowFileSelect(path, ModelFilters))
			*wstring = path.wstring();
}


REGISTER_COMPONENT(Model, STR_TO_WSTR(ICON_PYRAMID) + L" Model");

}
