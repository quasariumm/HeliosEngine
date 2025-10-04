#pragma once
#include "Component.h"
#include "imgui.h"
#include "Assets/ModelFileHandler.h"
#include "Core/Common.h"


namespace Engine
{
struct ModelData;

static void DisplayModelPath(const ComponentProperty* property);

class Model final : public Component
{

public:

	Model() : Component(this)
	{
		AssignProperty(L"Model Path", &modelPath, &DisplayModelPath);
		modelInstance = { nullptr, {} };
	}

	~Model() override
	{
		ObjectRenderer::Instance().DeregisterModelInstance(&modelInstance);
	}

	void OnLoad() override
	{
		DebugLog(LogSeverity::INFO, L"Init::Model");
		ObjectRenderer::Instance().RegisterModelInstance(m_attachedObject->GetTransform(), &modelInstance);

		Component::OnLoad();
	}

	void Tick() override
	{
		Component::Tick();

		if (m_attachedObject->GetTransformChanged())
			ObjectRenderer::Instance().UpdateModelTransforms();

		// Check if the model path has changed. If so, load the model
		if (modelPath != m_cachedModelPath)
		{
			m_cachedModelPath = modelPath;
			modelInstance = ModelFileHandler::LoadModel(modelPath);
			ObjectRenderer::Instance().UpdateModelSSBOs();
		}
	}

	void DisplayProperties() override
	{
		Component::DisplayProperties();

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

inline void DisplayModelPath(const ComponentProperty* property)
{
	auto* wstring = static_cast<std::wstring*>(property->value);
	std::filesystem::path path{*wstring};
	if (ImGui::Button("Browse"))
		if (ProjectHandler::ShowFileSelect(path, ModelFilters))
			*wstring = path.wstring();
	ImGui::SameLine();
	ImGui::Text("%s", path.string().c_str());
}


REGISTER_COMPONENT(Model, STR_TO_WSTR(ICON_PYRAMID) + L" Model");

}
