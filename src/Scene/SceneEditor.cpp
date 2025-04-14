#include "SceneEditor.h"

void Engine::SceneEditor::DrawInterface()
{
    TreeEditor();
    ObjectEditor();
}

void Engine::SceneEditor::TreeEditor()
{
    ImGui::Begin("Scene");

    if (ImGui::Button("New Empty"))
        m_targetScene->NewObject();

    if (m_selectMode == PARENT && m_prvSelectedObject != m_selectedObject)
    {
        m_targetScene->GetSceneObject(m_prvSelectedObject)->SetParent(m_targetScene->GetSceneObject(m_selectedObject));
        m_selectMode = SELECT;
    }

    for (SceneObject* object : m_targetScene->GetSceneObjectList())
        if (object->GetParent() == nullptr)
            DisplayObjectTree(object);

    ImGui::End();
}

void Engine::SceneEditor::ObjectEditor()
{
    ImGui::Begin("Object");

    // Show text of no object selected
    if (m_selectedObject == 0)
    {
        ImGui::Text("Please select an object...");
        ImGui::End();
        return;
    }

    // Make sure correct object is in variable
    static SceneObject* selectedObject = nullptr;

    if (selectedObject == nullptr)
        selectedObject = m_targetScene->GetSceneObject(m_selectedObject);

    if (selectedObject->GetUID() != selectedObject->GetParent()->GetUID())
        selectedObject = m_targetScene->GetSceneObject(m_selectedObject);

    // Show basic info
    ImGui::Text(selectedObject->GetName().c_str());
    ImGui::Text("UID: %u", selectedObject->GetUID());

    if (ImGui::Button("Set Parent"))
    {
        m_prvSelectedObject = m_selectedObject;
        m_selectMode = PARENT;
    }

    // Renaming
    ImGui::SameLine();
    if (ImGui::Button("Rename"))
        ImGui::OpenPopup("RenameSceneObject");

    if (ImGui::BeginPopupModal("RenameSceneObject", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static char newName[64] = "";
        ImGui::InputText("New name", newName, 64);
        if (ImGui::Button("Set new name"))
        {
            selectedObject->SetName(std::string(newName));
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // Deleting
    ImGui::SameLine();
    if (ImGui::Button("Delete"))
        m_targetScene->DeleteObject(selectedObject);

    if (ImGui::CollapsingHeader("Transform"))
        selectedObject->GetTransform()->TransformControllerUI();

    for (Component* c : selectedObject->GetComponentList())
        if (ImGui::CollapsingHeader(c->GetName().c_str()))
            c->DisplayProperties();

    if (ImGui::Button("Add component"))
        ImGui::OpenPopup("AddComponentToObject");

    if (ImGui::BeginPopupModal("AddComponentToObject", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        // if (ImGui::Button("Sphere"))
        // {
        //     Sphere* s = new Sphere();
        //     selectedObject->AddComponent(&s->mComponent);
        //     ImGui::CloseCurrentPopup();
        // }

        ImGui::EndPopup();
    }

    ImGui::End();
}

void Engine::SceneEditor::DisplayObjectTree(SceneObject* object)
{
    // Display parent object
    if (ImGui::Selectable(object->GetName().c_str(), m_selectedObject == object->GetUID()))
        m_selectedObject = object->GetUID();

    ImGui::Indent();
    // Display each child object
    for (SceneObject* child : object->GetChildren())
        DisplayObjectTree(child);
    ImGui::Unindent();
}