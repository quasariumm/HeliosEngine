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
		modelInstance = { nullptr, {} };
	}

	~Model() override
	{
		ObjectRenderer::Instance().DeregisterModelInstance(&modelInstance);
	}

	void Init() override
	{
		ObjectRenderer::Instance().RegisterModelInstance(m_attachedObject->GetTransform(), &modelInstance);
	}

	void DisplayProperties() override
	{
		Component::DisplayProperties();

		if (m_attachedObject->GetTransformChanged())
			ObjectRenderer::Instance().UpdateModelTransforms();

		// Check if the model path has changed. If so, load the model
		if (modelPath != m_cachedModelPath)
		{
			m_cachedModelPath = modelPath;
			modelInstance = ModelFileHandler::LoadModel(modelPath);
			ObjectRenderer::Instance().UpdateModelSSBOs();
		}

		if (modelInstance.modelData == nullptr) return;
		// Display the meshes and their properties
		for (int& index : modelInstance.materialIndices)
		{
			ImGui::PushID(&index);
			if (ImGui::TreeNode("Mesh"))
			{
				if (ImGui::InputInt("Material Index", &index))
					ObjectRenderer::Instance().UpdateModelSSBOs();
				ImGui::TreePop();
			}
			ImGui::PopID();
		}
	}

	std::wstring modelPath;

	ModelInstance modelInstance;

private:

	std::wstring m_cachedModelPath;

};

inline void DisplayModelPath(void* data)
{
	auto* wstring = (std::wstring*)data;
	std::filesystem::path path{*wstring};
	if (ImGui::Button("Browse"))
		if (ProjectHandler::ShowFileSelect(path, ModelFilters))
			*wstring = path.wstring();
	ImGui::SameLine();
	ImGui::Text(path.string().c_str());
}


REGISTER_COMPONENT(Model, STR_TO_WSTR(ICON_PYRAMID) + L" Model");

}
