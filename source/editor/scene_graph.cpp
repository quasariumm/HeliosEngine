#include "editor/editor_menus.hpp"

#include "components/basic_components.hpp"
#include "core/ecs.hpp"
#include "tools/scene.hpp"

using namespace Engine;
using namespace Editor;
using namespace Components;
using namespace entt;
using namespace std;

SceneObject SceneGraph::selected = entt::null;

void SceneGraph::Draw()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    if (!ImGui::Begin(ICON_GRAPH" Scene Graph"))
    {
        ImGui::PopStyleVar();
        return;
    }

    if (ImGui::BeginTable("SceneObjects", 3, ImGuiTableFlags_RowBg | ImGuiTableFlags_Hideable | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY))
    {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("ID");
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
        ImGui::TableSetupColumn(ICON_EYE, ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, ImGui::GetFontSize() * 1.2f);
        ImGui::TableHeadersRow();

        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        bool open = ImGui::TreeNodeEx("##Scene", ImGuiTreeNodeFlags_DrawLinesFull | ImGuiTreeNodeFlags_DefaultOpen);

        ImGui::TableNextColumn();
        if (ImGui::Selectable("Scene")) selected = entt::null;
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_OBJECT_REPARENT"))
                ReParent(entt::null, *static_cast<entt::entity*>(payload->Data));
            ImGui::EndDragDropTarget();
        }
        ImGui::TableNextColumn();

        if (open)
        {
            auto root_objects = Systems::GetECS()->Registry()->view<SceneObjectInfo, ParentObject>(entt::exclude<ChildObject>);
            for(const auto& [entity, scene, parent]: root_objects.each())
                DrawObjectGraph(entity);
            ImGui::TreePop();
        }
        
        ImGui::EndTable();
    }
    
    ImGui::End();
    ImGui::PopStyleVar();
}

void SceneGraph::DrawObjectGraph(entity entity)
{
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    
    const std::string id = to_string(static_cast<uint32_t>(entity));
    auto scene_object = GetComponent<SceneObjectInfo>(entity);
    std::string name = scene_object.name;
    bool is_visible = IsVisible(entity);
    vector<entt::entity> children = GetComponent<ParentObject>(entity).children;

    ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_DrawLinesFull;
    if (children.empty())
        tree_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet;
    else
        tree_flags |= ImGuiTreeNodeFlags_DefaultOpen;

    // Show row
    ImGui::PushID((name + id).c_str());
    bool open = ImGui::TreeNodeEx(("##" + id).c_str(), tree_flags);
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

