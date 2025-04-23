#include "SceneObjects.h"

#include "Debugger/Debugger.h"

namespace Engine
{
void Transform::TransformControllerUI()
{
    ImGui::DragFloat3("Position", p.cell, 0.1f);
    ImGui::DragFloat3("Rotation", r.cell, 1.0f);
    ImGui::DragFloat3("Scale", s.cell, 0.1f);
    UpdateTransform();
}

SceneObject::~SceneObject()
{
    // Make sure there are no more references
    for (SceneObject* child : m_childObjects)
    {
        child->SetParent(nullptr);
        RemoveChild(child);
    }
    SetParent(nullptr);
}

mat4 SceneObject::GlobalMatrix() const
{
    if (m_parentObject != nullptr)
        return m_parentObject->GlobalMatrix() * m_transform.matrix();
    return m_transform.matrix();
}

void SceneObject::SetParent(SceneObject* object)
{
    if (m_parentObject != nullptr)
        m_parentObject->RemoveChild(this);
    m_parentObject = object;
    if (object != nullptr)
        object->m_childObjects.push_back(this);
}

void SceneObject::RemoveChild(const SceneObject* object)
{
    for (int i = 0; i < (int)m_childObjects.size(); i++)
        if (m_childObjects[i] == object)
            m_childObjects.erase(m_childObjects.begin() + i);
}

Component* SceneObject::AddComponentByName(const std::wstring& name)
{
    std::unique_ptr<Component> comp = ComponentRegister::Instance().Create(name);
    if (comp)
    {
        Component* ptr = comp.get();
        m_components.push_back(std::move(comp));
        return ptr;
    }

    std::wcerr << "Component not found: " << name << std::endl;
    return nullptr;
}
}

