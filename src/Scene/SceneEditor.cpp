#include "SceneEditor.h"

void Engine::DisplayObjectTree(SceneObject* object)
{
    // // Display parent object
    // if (ImGui::Selectable(object->GetName().c_str(), gSelectedObject == object->GetUID()))
    //     gSelectedObject = object->GetUID();
    //
    // ImGui::Indent();
    // // Display each child object
    // for (SceneObject* child : object->GetChildren())
    //     DisplayObjectTree(child);
    // ImGui::Unindent();
}

void Engine::SceneEditor::SceneTreeEditor() const
{
    // ImGui::Begin("Scene Editor");
    //
    // if (ImGui::Button("New Empty"))
    //     mTargetScene->NewObject();
    //
    // if (gSelectMode == PARENT && gPreviousSelectedObject != gSelectedObject)
    // {
    //     mTargetScene->GetSceneObject(gPreviousSelectedObject)->SetParent(mTargetScene->GetSceneObject(gSelectedObject));
    //     gSelectMode = SELECT;
    // }
    //
    // for (SceneObject* object : mTargetScene->GetSceneObjectList())
    //     if (object->GetParent() == nullptr)
    //         DisplayObjectTree(object);
    //
    // ImGui::End();
}

void Engine::SceneEditor::ObjectEditor() const
{
    // ImGui::Begin("Object Editor");
    //
    // // Show text of no object selected
    // if (gSelectedObject == 0)
    // {
    //     ImGui::Text("Please select an object...");
    //     ImGui::End();
    //     return;
    // }
    //
    // // Make sure correct object is in variable
    // static SceneObject* selectedObject = nullptr;
    //
    // if (selectedObject == nullptr)
    //     selectedObject = mTargetScene->GetSceneObject(gSelectedObject);
    //
    // if (selectedObject->GetUID() != gSelectedObject)
    //     selectedObject = mTargetScene->GetSceneObject(gSelectedObject);
    //
    // // Show basic info
    // ImGui::Text(selectedObject->GetName().c_str());
    // ImGui::Text("UID: %u", selectedObject->GetUID());
    //
    // if (ImGui::Button("Set Parent"))
    // {
    //     gPreviousSelectedObject = gSelectedObject;
    //     gSelectMode = PARENT;
    // }
    //
    // // Renaming
    // ImGui::SameLine();
    // if (ImGui::Button("Rename"))
    //     ImGui::OpenPopup("RenameSceneObject");
    //
    // if (ImGui::BeginPopupModal("RenameSceneObject", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    // {
    //     static char newName[64] = "";
    //     ImGui::InputText("New name", newName, 64);
    //     if (ImGui::Button("Set new name"))
    //     {
    //         selectedObject->SetName(std::string(newName));
    //         ImGui::CloseCurrentPopup();
    //     }
    //     ImGui::EndPopup();
    // }
    //
    // // Deleting
    // ImGui::SameLine();
    // if (ImGui::Button("Delete"))
    //     mTargetScene->DeleteObject(selectedObject);
    //
    // if (ImGui::CollapsingHeader("Transform"))
    //     selectedObject->GetTransform()->TransformControllerUI();
    //
    // for (Component* c : selectedObject->GetComponentList())
    //     if (ImGui::CollapsingHeader(c->GetName().c_str()))
    //         c->DisplayProperties();
    //
    // if (ImGui::Button("Add component"))
    //     ImGui::OpenPopup("AddComponentToObject");
    //
    // if (ImGui::BeginPopupModal("AddComponentToObject", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    // {
    //     // if (ImGui::Button("Sphere"))
    //     // {
    //     //     Sphere* s = new Sphere();
    //     //     selectedObject->AddComponent(&s->mComponent);
    //     //     ImGui::CloseCurrentPopup();
    //     // }
    //
    //     ImGui::EndPopup();
    // }
    //
    // ImGui::End();
}
