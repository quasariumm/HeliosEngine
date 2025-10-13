#include "tools/scene.hpp"
#include "components/basic_components.hpp"

using namespace Engine;
using namespace Components;
using namespace entt;

template <typename T>
T& Engine::AddComponent(const SceneObject& object)
{
    if (HasComponent<T>(object))
    {
        Log::Error("Object already has the requested Components!");
    }

    return Systems::GetECS()->Registry()->emplace<T>(object);
}

template <typename T>
void Engine::RemoveComponent(const SceneObject& object)
{
    if (!HasComponent<T>(object))
    {
        Log::Error("Object does not have the Components that was requested to be removed!");
        return;
    }

    Systems::GetECS()->Registry()->remove<T>(object);
}

template <typename T>
T& Engine::GetComponent(const SceneObject& object)
{
    if (!HasComponent<T>(object))
    {
        Log::Error("Object does not have the requested Component!");
    }

    return Systems::GetECS()->Registry()->get<T>(object);
}

template <typename T>
T* Engine::TryGetComponent(const SceneObject& object)
{
    return Systems::GetECS()->Registry()->try_get<T>(object);
}

template <typename T>
bool Engine::HasComponent(const SceneObject& object)
{
    return Systems::GetECS()->Registry()->try_get<T>(object);
}

void Engine::ReParent(const SceneObject& parent, const SceneObject& child)
{
    // Make sure that you cannot make an objects parent their child
    if (IsChildOf(child, parent)) return;
    auto* reg = Systems::GetECS()->Registry();
    // Make sure the child is no longer set as a child of its current parent
    ChildObject* child_comp = reg->try_get<ChildObject>(child);
    if (child_comp != nullptr)
        reg->get<ParentObject>(child_comp->parent).RemoveChild(child);

    if (parent == entt::null)
    {
        // Object no longer has a parent so remove the child object Components
        reg->remove<ChildObject>(child);
    }
    else
    {
        // Set the new parent of the child
        reg->get_or_emplace<ChildObject>(child).parent = parent;

        // Set the child as a child of the new parent
        reg->get<ParentObject>(parent).AddChild(child);
    }
}

bool Engine::IsChildOf(const SceneObject& parent, const SceneObject& possibleChild) // NOLINT(*-no-recursion)
{
    const auto* c = TryGetComponent<ChildObject>(possibleChild);
    if (!c) return false; // Has no child Components, so can not be a child of parent
    if (c->parent == entt::null) return false; // Has no parent set, so can not be a child of parent
    if (c->parent == parent) return true; // Parent is the requested parent
    return IsChildOf(parent, c->parent); // Checking recursively upward
}

bool Engine::IsVisible(const SceneObject& sceneObject) // NOLINT(*-no-recursion)
{
    for (const auto& [e] : Systems::GetECS()->Registry()->view<Hidden>().each()) if (e == sceneObject) return false;
    const auto* child = TryGetComponent<ChildObject>(sceneObject);
    if (child != nullptr) return IsVisible(child->parent);
    return true;
}

void Engine::SetVisible(const SceneObject& sceneObject, bool visible)
{
    if (IsVisible(sceneObject) == visible) return;

    if (visible)
        Systems::GetECS()->Registry()->remove<Hidden>(sceneObject);
    else
        Systems::GetECS()->Registry()->emplace<Hidden>(sceneObject);

}

glm::mat4 Engine::GetGlobalTransform(const SceneObject& sceneObject) // NOLINT(*-no-recursion)
{
    const auto* child = TryGetComponent<ChildObject>(sceneObject);
    const glm::mat4 local = GetComponent<Transform>(sceneObject).GetMatrix();
    if (child == nullptr)
        return local;

    // Calling this function recursively to get global matrix
    // TODO(Quillan): Look for possible better alternative, could be overkill to do every frame if parent doesn't move
    return GetGlobalTransform(child->parent) * local;
}

SceneObject Engine::CreateSceneObject(const std::string& name)
{
    auto* reg = Systems::GetECS()->Registry();

    const SceneObject newObject = reg->create();
    reg->emplace<SceneObjectInfo>(newObject, name);
    reg->emplace<Transform>(newObject);
    reg->emplace<ParentObject>(newObject);
    return newObject;
}

void Engine::DestroySceneObject(const SceneObject& object)
{
    Systems::GetECS()->Registry()->emplace<DeleteMarker>(object);
}

void Engine::DestroyMarkedSceneObjects()
{
    const auto view = Systems::GetECS()->Registry()->view<DeleteMarker>();
    for (auto [e] : view.each())
        Systems::GetECS()->Registry()->destroy(e);
}
