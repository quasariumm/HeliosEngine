#include "editor/editor_menus.hpp"

#include "components/basic_components.hpp"
#include "core/ecs.hpp"
#include "tools/scene.hpp"

using namespace Engine;
using namespace Editor;
using namespace Components;

SceneObject SceneGraph::selected = entt::null;


void SceneGraph::Draw()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	if (!ImGui::Begin(ICON_GRAPH" Scene Graph"))
	{
		ImGui::PopStyleVar();
		return;
	}

	if (ImGui::BeginTable("SceneObjects", 3,
	                      ImGuiTableFlags_RowBg | ImGuiTableFlags_Hideable | ImGuiTableFlags_Resizable |
	                      ImGuiTableFlags_ScrollY))
	{
		ImGui::TableSetupScrollFreeze(0, 1);
		ImGui::TableSetupColumn("ID");
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
		ImGui::TableSetupColumn(ICON_EYE, ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize,
		                        ImGui::GetFontSize() * 1.2f);
		ImGui::TableHeadersRow();

		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		const bool open = ImGui::TreeNodeEx(
				"##Scene", ImGuiTreeNodeFlags_DrawLinesFull | ImGuiTreeNodeFlags_DefaultOpen);

		ImGui::TableNextColumn();
		if (ImGui::Selectable("Scene"))
			selected = entt::null;
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_OBJECT_REPARENT"))
				ReParent(entt::null, *static_cast<entt::entity*>(payload->Data));
			ImGui::EndDragDropTarget();
		}
		ImGui::TableNextColumn();

		if (open)
		{
			for (const auto rootObjects = ECS::Registry()->view<SceneObjectInfo, ParentObject>(
					     entt::exclude<ChildObject>);
			     const auto& [entity, scene, parent] : rootObjects.each())
				DrawObjectGraph(entity);
			ImGui::TreePop();
		}

		ImGui::EndTable();
	}

	ImGui::End();
	ImGui::PopStyleVar();
}


void SceneGraph::DrawObjectGraph( SceneObject entity )
{
	ImGui::TableNextRow();
	ImGui::TableNextColumn();

	const std::string              id          = std::to_string(static_cast<uint32_t>(entity));
	const auto                     sceneObject = Engine::GetComponent<SceneObjectInfo>(entity);
	const std::string              name        = sceneObject.name;
	const bool                     isVisible   = IsVisible(entity);
	const std::vector<SceneObject> children    = GetComponent<ParentObject>(entity).children;

	ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_DrawLinesFull;
	if (children.empty())
		treeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet;
	else
		treeFlags |= ImGuiTreeNodeFlags_DefaultOpen;

	// Show row
	ImGui::PushID((name + id).c_str());
	const bool open = ImGui::TreeNodeEx(("##" + id).c_str(), treeFlags);
	ImGui::TableNextColumn();
	if (ImGui::Selectable(name.c_str(), selected == entity))
		selected = entity;

	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload("SCENE_OBJECT_REPARENT", &entity, sizeof(entt::entity));
		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_OBJECT_REPARENT"))
			ReParent(entity, *static_cast<entt::entity*>(payload->Data));
		ImGui::EndDragDropTarget();
	}

	ImGui::TableNextColumn();
	const ImVec2 cPos = ImGui::GetCursorPos();
	if (ImGui::InvisibleButton("##IsVisible", {ImGui::GetFontSize(), ImGui::GetFontSize()}))
		SetVisible(entity, !isVisible);
	ImGui::SetCursorPos(cPos);
	if (isVisible)
		ImGui::Text(ICON_EYE);
	else
		ImGui::TextDisabled(ICON_EYE_OFF);

	// If it has children, draw them as well
	if (!children.empty() && open)
	{
		for (const entt::entity child : children)
			DrawObjectGraph(child);
		ImGui::TreePop();
	}

	// Only pop tree if it is a leaf node
	if (children.empty())
		ImGui::TreePop();

	ImGui::PopID();
}
