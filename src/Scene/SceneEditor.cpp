#include "SceneEditor.h"

#include "SceneStorage.h"
#include "Core/IconsFA.h"

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
    ImGui::Begin(ICON_FA_MAP" Scene", nullptr, ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar())
    {
        if (m_sceneFile.empty())
        {
            ImGui::Text(ICON_FA_CIRCLE_EXCLAMATION " No scene file loaded");
            ImGui::EndMenuBar();
            ImGui::End();
            return;
        }

        std::string sceneName = "File";
        if (!m_sceneFile.empty())
        {
            sceneName = m_sceneFile.filename().string();
            sceneName = sceneName.substr(0, sceneName.size() - 4);
        }

        if (ImGui::BeginMenu(sceneName.c_str()))
        {
            if (ImGui::MenuItem("Save"))
                SceneLoader::SaveToFile(m_targetScene, m_sceneFile);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Object"))
        {
            if (ImGui::MenuItem("New"))
                m_targetScene->NewObject();
            ImGui::EndMenu();
        }

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

    ImGui::End();
}

void SceneEditor::ObjectEditor()
{
    ImGui::Begin(ICON_FA_CUBE" Object Viewer");

    // Show text of no object selected
    if (m_selectedObject == 0)
    {
        ImGui::Text("Please select an object...");
        ImGui::End();
        return;
    }

    // Get the actual object
    SceneObject* selectedObject = m_targetScene->GetSceneObject(m_selectedObject);

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
        ImGui::OpenPopup("Rename Object");

    if (ImGui::BeginPopupModal("Rename Object", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
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
    {
        m_selectedObject = 0;
        m_prvSelectedObject = 0;
        m_targetScene->DeleteObject(selectedObject);
    }

    if (ImGui::CollapsingHeader("Transform"))
        selectedObject->GetTransform()->TransformControllerUI();

    for (Component* c : selectedObject->GetComponentList())
        if (ImGui::CollapsingHeader(c->GetName().c_str()))
            c->DisplayProperties();

    if (ImGui::Button("Add component"))
        ImGui::OpenPopup("Select component");

    if (ImGui::BeginPopupModal("Select component", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        // if (ImGui::Button("Sphere"))
        // {
        //     Sphere* s = new Sphere();
        //     selectedObject->AddComponent(&s->mComponent);
        //     ImGui::CloseCurrentPopup();
        // }

        if (ImGui::Button("Cancel"))
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }

    ImGui::End();
}

void Engine::SceneEditor::DisplayObjectTree(SceneObject* object)
{
    // Display parent object
    ImGui::PushID((int)object->GetUID());

    std::string prefix = ICON_FA_CUBE;
    if (m_selectMode == PARENT && m_prvSelectedObject == object->GetUID())
        prefix += ICON_FA_ARROW_RIGHT;

    if (ImGui::Selectable((prefix + " " + object->GetName()).c_str(), m_selectedObject == object->GetUID()))
        m_selectedObject = object->GetUID();
    ImGui::PopID();

    ImGui::Indent();
    // Display each child object
    for (SceneObject* child : object->GetChildren())
        DisplayObjectTree(child);
    ImGui::Unindent();
}

}
