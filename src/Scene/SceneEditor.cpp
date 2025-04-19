#include "SceneEditor.h"

#include "SceneStorage.h"

namespace Engine
{
Scene* SceneEditor::m_targetScene = nullptr;
std::filesystem::path SceneEditor::m_sceneFile = "";

void SceneEditor::DrawInterface()
{
    TreeEditor();
    ObjectEditor();
}

void SceneEditor::TreeEditor()
{
    ImGui::Begin(ICON_MAP" Scene", nullptr, ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar())
    {
        if (m_sceneFile.empty())
        {
            ImGui::Text(ICON_ALERT" No scene file loaded");
            ImGui::EndMenuBar();
            ImGui::End();
            return;
        }

        if (ImGui::MenuItem(ICON_CONTENT_SAVE))
            SceneLoader::SaveToFile(m_targetScene, m_sceneFile);

        ImGui::Separator();

        if (ImGui::MenuItem(ICON_PLUS_THICK))
            m_targetScene->NewObject();

        ImGui::EndMenuBar();
    }

    if (m_selectMode == PARENT)
    {
        ImGui::TextColored({1.0f, 1.0f, 0.0f, 1.0f}, "Click object to set as parent");
        if (ImGui::Button("Cancel"))
            m_selectMode = SELECT;
        ImGui::Separator();
    }

    if (m_selectMode == PARENT && m_prvSelectedObject != m_selectedObject)
    {
        m_targetScene->GetSceneObject(m_prvSelectedObject)->SetParent(m_targetScene->GetSceneObject(m_selectedObject));
        m_selectMode = SELECT;
    }

    for (SceneObject* object : m_targetScene->GetSceneObjectList())
        if (object->GetParent() == nullptr)
            DisplayObjectTree(object);

    // Renaming
    if (ImGui::BeginPopupModal("Rename Object", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        if (m_renameObject == nullptr)
            ImGui::CloseCurrentPopup();

        static char newName[64] = "";
        ImGui::InputText("New name", newName, 64);
        if (ImGui::Button("Set new name"))
        {
            m_renameObject->SetName(std::string(newName));
            m_renameObject = nullptr;
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::Button("Cancel"))
        {
            m_renameObject = nullptr;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    } else if (m_renameObject != nullptr)
        ImGui::OpenPopup("Rename Object");

    ImGui::End();
}

void SceneEditor::ObjectEditor()
{
    ImGui::Begin(ICON_CUBE_SCAN" Object Viewer", nullptr, ImGuiWindowFlags_MenuBar);

    // Get the actual object
    SceneObject* selectedObject = nullptr;
    if (m_selectedObject != 0)
        selectedObject = m_targetScene->GetSceneObject(m_selectedObject);

    if (ImGui::BeginMenuBar())
    {
        // Show text of no object selected
        if (m_selectedObject == 0)
            ImGui::Text(ICON_CUBE" No object selected");
        else
            ImGui::Text((std::string(ICON_CUBE) + " " + selectedObject->GetName()).c_str());

        ImGui::EndMenuBar();
    }

    // Make sure the object exists in the scene
    // ReSharper disable once CppDFAConstantConditions
    if (selectedObject == nullptr)
    {
        m_selectedObject = 0;
        ImGui::End();
        return;
    }

    // Show basic info
    // ReSharper disable once CppDFAUnreachableCode
    ImGui::BeginDisabled();
    ImGui::Text("UID: %u", selectedObject->GetUID());
    ImGui::EndDisabled();

    ImGui::Separator();

    if (ImGui::CollapsingHeader("Transform"))
        selectedObject->GetTransform()->TransformControllerUI();

    for (const std::unique_ptr<Component>& c : selectedObject->GetComponentList())
        if (ImGui::CollapsingHeader(c->GetName().c_str()))
            c->DisplayProperties();

    ImGui::Separator();

    if (ImGui::Button("Add component"))
        ImGui::OpenPopup("Select component");

    if (ImGui::BeginPopupModal("Select component", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        for (const std::string& componentName : ComponentRegister::Instance().GetComponentNames())
        {
            if (ImGui::Button(componentName.c_str()))
            {
                selectedObject->AddComponentByName(componentName);
                ImGui::CloseCurrentPopup();
            }
        }

        if (ImGui::Button("Cancel"))
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }

    ImGui::End();
}

void SceneEditor::DisplayObjectTree(SceneObject* object)
{
    // Display parent object
    ImGui::PushID((int)object->GetUID());

    std::string prefix = ICON_CUBE;
    if (m_selectMode == PARENT && m_prvSelectedObject == object->GetUID())
        prefix += ICON_ARROW_RIGHT_THICK;

    if (ImGui::Selectable((prefix + " " + object->GetName()).c_str(), m_selectedObject == object->GetUID()))
        m_selectedObject = object->GetUID();

    if (ImGui::BeginPopupContextItem())
    {
        m_selectedObject = object->GetUID();
        if (ImGui::Selectable(ICON_DELETE" Delete"))
        {
            m_selectedObject = 0;
            m_prvSelectedObject = 0;
            m_targetScene->DeleteObject(object);
        }
        if (ImGui::Selectable(ICON_RENAME" Rename"))
            m_renameObject = object;
        if (ImGui::Selectable(ICON_FAMILY_TREE" Re-parent"))
        {
            m_prvSelectedObject = m_selectedObject;
            m_selectMode = PARENT;
        }
        ImGui::EndPopup();
    }

    ImGui::PopID();

    ImGui::Indent();
    // Display each child object
    for (SceneObject* child : object->GetChildren())
        DisplayObjectTree(child);
    ImGui::Unindent();
}

REGISTER_EDITOR_INTERFACE(SceneEditor);

}
