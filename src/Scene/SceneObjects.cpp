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

template <typename T>
T* SceneObject::FindComponent()
{
    for (const std::unique_ptr<Component>& c : m_components)
        if (c->GetType() == typeid(T))
            return static_cast<T*>(c);
    DebugLog(LogSeverity::WARNING, "Object did not have requested component");
    return nullptr;
}

template <typename T>
T* SceneObject::AddComponent()
{
    auto component = std::make_unique<T>();
    T* ptr = component.get(); // raw pointer for access
    m_components.push_back(std::move(component));
    return ptr;
}

Component* SceneObject::AddComponentByName(const std::string& name)
{
    std::unique_ptr<Component> comp = ComponentRegister::Instance().Create(name);
    if (comp)
    {
        Component* ptr = comp.get();
        m_components.push_back(std::move(comp));
        return ptr;
    }

    std::cerr << "Component not found: " << name << std::endl;
    return nullptr;
}
}

