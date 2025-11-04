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
	if (!ImGui::Begin(ICON_GRAPH" Scene Graph", nullptr, ImGuiWindowFlags_MenuBar))
	{
		ImGui::PopStyleVar();
		ImGui::End();
		return;
	}

	ImGui::PopStyleVar();

	if (ImGui::BeginMenuBar())
	{
		// TODO(Quillan): Scene saving/loading
		// if (ImGui::MenuItem(ICON_CONTENT_SAVE))
		// 	ECS::Get()->SaveScene();
		// ImGui::SetItemTooltip("Save the current scene");
		//
		// if (ImGui::MenuItem(ICON_UPLOAD))
		// {
		// 	ECS::Get()->loadedScene = resources::LoadResource<resources::Scene>(Directory::Assets, "test.scn");
		// 	ECS::Get()->LoadScene();
		// }
		// ImGui::SetItemTooltip("Load scene from file");

		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::GetFontSize() * 2.0f);
		if (ImGui::BeginMenu(ICON_PLUS_THICK))
		{
			if (ImGui::MenuItem(ICON_CUBE" Empty Object")) CreateSceneObject("Object");
			// TODO(Quillan & Patrick): 3D Model Loading
			// if (ImGui::BeginMenu(ICON_CUBE_SCAN" 3D Model"))
			// {
			// 	static char file[256];
			// 	ImGui::InputText("File", file, 256);
			// 	if (ImGui::Button("Load"))
			// 		LoadRenderObject("3D Model", file, selected);
			// 	ImGui::EndMenu();
			// }
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if (ImGui::TreeNodeEx("Scene", ImGuiTreeNodeFlags_DrawLinesFull | ImGuiTreeNodeFlags_DefaultOpen))
	{
		auto root_objects = ECS::Registry()->view<SceneObjectInfo, ParentObject>(entt::exclude<ChildObject>);
		for(const auto& [entity, scene, parent]: root_objects.each())
			DrawObjectGraph(entity);
		ImGui::TreePop();
	}

	ImGui::End();
}


void SceneGraph::DrawObjectGraph(SceneObject entity)
{
    const std::string id = std::to_string(static_cast<uint32_t>(entity));
    SceneObjectInfo* scene_object = &ECS::Registry()->get<SceneObjectInfo>(entity);
    std::string name = scene_object->name;
    bool is_visible = IsVisible(entity);
    std::vector<entt::entity> children = ECS::Registry()->get<ParentObject>(entity).children;

    ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_DrawLinesFull;
    if (children.empty())
        tree_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet;
    else
        tree_flags |= ImGuiTreeNodeFlags_DefaultOpen;

    // Show row
    ImGui::PushID((name + id).c_str());
    bool open = ImGui::TreeNodeEx(("##" + id).c_str(), tree_flags);
    ImGui::SameLine();
    if (ImGui::Selectable(name.c_str(), selected == entity, ImGuiSelectableFlags_None, {ImGui::GetContentRegionAvail().x - ImGui::GetFontSize() * 2.2f, 0.0f}))
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

    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - ImGui::GetFontSize() * 1.5f);
    ImVec2 c_pos = ImGui::GetCursorPos();
    if (ImGui::InvisibleButton("##IsVisible", {ImGui::GetFontSize(), ImGui::GetFontSize()}))
        SetVisible(entity, !is_visible);
    ImGui::SetCursorPos(c_pos);
    if (is_visible)
        ImGui::Text(ICON_EYE);
    else
        ImGui::TextDisabled(ICON_EYE_OFF);

    // If it has children, draw them as well
    if (!children.empty() && open)
    {
        for (entt::entity child : children)
            DrawObjectGraph(child);
        ImGui::TreePop();
    }

    // Only pop tree if it is a leaf node
    if (children.empty())
        ImGui::TreePop();

    ImGui::PopID();
}
