#include "editor/editor_menus.hpp"

#include "components/basic_components.hpp"
#include "core/ecs.hpp"

using namespace Engine;
using namespace Editor;
using namespace Components;


void EntityInspector::Draw()
{
	static entt::entity lastTarget   = entt::null;
	static char         newName[64]  = "";
	static bool         changingName = false;

	const float fontSize = ImGui::GetFontSize();

	const entt::entity targetEntity = SceneGraph::selected;
	if (lastTarget != targetEntity)
	{
		changingName = false;
	}

	lastTarget = targetEntity;
	ImGui::Begin(ICON_MAGNIFY_SCAN" Inspector");
	if (targetEntity == entt::null)
	{
		ImGui::End();
		return;
	}

	// General information
	auto compSceneObject = GetComponent<SceneObjectInfo>(targetEntity);
	if (changingName)
	{
		if (ImGui::Button(ICON_CHECK))
		{
			compSceneObject.name = newName;
			changingName          = false;
		}
		ImGui::SameLine();
		ImGui::InputText("##New Name", newName, 64);
	}
	else
	{
		if (ImGui::Button(ICON_PENCIL))
		{
			strncpy_s(newName, compSceneObject.name.c_str(), 64);
			changingName = true;
		}
		ImGui::SameLine();
		ImGui::SetCursorPosX(
				ImGui::GetWindowSize().x * 0.5f - ImGui::CalcTextSize(compSceneObject.name.c_str()).x * 0.5f);
		ImGui::Text("%s", compSceneObject.name.c_str());

		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowSize().x - fontSize * 2.0f);
		if (ImGui::Button(ICON_TRASH_CAN"##DeleteObject"))
		{
			DestroySceneObject(SceneGraph::selected);
			SceneGraph::selected = entt::null;
		}
	}

	for (const auto& c : ECS::Get()->inspectableComponents)
		c(targetEntity);

	ImGui::SetCursorPosY(ImGui::GetCursorPos().y + fontSize);
	ImGui::Separator();
	ImGui::SetCursorPosY(ImGui::GetCursorPos().y + fontSize);

	if (ImGui::Button(ICON_PLUS" Add Component", ImVec2(ImGui::GetContentRegionAvail().x, fontSize * 1.5f)))
		ImGui::OpenPopup("Add Component");

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(fontSize, fontSize));
	if (ImGui::BeginPopup("Add Component"))
	{
		for (const auto& c : ECS::Get()->addableComponents)
			c(targetEntity);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + fontSize * 0.5f);
		ImGui::Separator();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + fontSize * 0.5f);
		if (ImGui::Button("Cancel", ImVec2(ImGui::GetContentRegionAvail().x, fontSize * 1.5f)))
			ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();

	ImGui::End();
}
