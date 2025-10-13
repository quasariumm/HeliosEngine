#include "editor/editor_menus.hpp"

#include "components/basic_components.hpp"
#include "core/ecs.hpp"

using namespace Engine;
using namespace Editor;
using namespace Components;

void EntityInspector::Draw()
{
    static entt::entity last_target = entt::null;
    static char new_name[64] = "";
    static bool changing_name = false;

    float f_s = ImGui::GetFontSize();
    
    entt::entity target_entity = SceneGraph::selected;
    if (last_target != target_entity)
    {
        changing_name = false;
    }
    
    last_target = target_entity;
    ImGui::Begin(ICON_MAGNIFY_SCAN" Inspector");
    if (target_entity == entt::null)
    {
        ImGui::End();
        return;
    }

    // General information
    SceneObjectInfo comp_scene_object = GetComponent<SceneObjectInfo>(target_entity);
    if (changing_name)
    {
        if (ImGui::Button(ICON_CHECK))
        {
            comp_scene_object.name = new_name;
            changing_name = false;
        }
        ImGui::SameLine();
        ImGui::InputText("##New Name", new_name, 64 );
    }
    else
    {
        if (ImGui::Button(ICON_PENCIL))
        {
            strncpy_s(new_name, comp_scene_object.name.c_str(), 64);
            changing_name = true;
        }
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f - ImGui::CalcTextSize(comp_scene_object.name.c_str()).x * 0.5f);
        ImGui::Text("%s", comp_scene_object.name.c_str());

        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowSize().x - f_s * 2.0f);
        if (ImGui::Button(ICON_TRASH_CAN"##DeleteObject"))
        {
            DestroySceneObject(SceneGraph::selected);
            SceneGraph::selected = entt::null;
        }
    }

    for (const auto& c : Systems::GetECS()->inspectableComponents)
        c(target_entity);

    ImGui::SetCursorPosY(ImGui::GetCursorPos().y + f_s);
    ImGui::Separator();
    ImGui::SetCursorPosY(ImGui::GetCursorPos().y + f_s);
    
    if (ImGui::Button(ICON_PLUS" Add Component", ImVec2(ImGui::GetContentRegionAvail().x, f_s * 1.5f)))
        ImGui::OpenPopup("Add Component");

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(f_s, f_s));
    if (ImGui::BeginPopup("Add Component"))
    {
        for (const auto& c : Systems::GetECS()->addableComponents)
            c(target_entity);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + f_s * 0.5f);
        ImGui::Separator();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + f_s * 0.5f);
        if (ImGui::Button("Cancel", ImVec2(ImGui::GetContentRegionAvail().x, f_s * 1.5f)))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();
    
    ImGui::End();
}
