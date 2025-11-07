#include "tools/scene.hpp"
#include "components/basic_components.hpp"

using namespace Helios;
using namespace Components;


void Helios::ReParent( const SceneObject& parent, const SceneObject& child )
{
	// Make sure that you cannot make an objects parent their child
	if (IsChildOf(child, parent))
		return;
	auto* reg = ECS::Registry();
	// Make sure the child is no longer set as a child of its current parent
	if (const ChildObject* childComp = reg->try_get<ChildObject>(child);
		childComp != nullptr)
		reg->get<ParentObject>(childComp->parent).RemoveChild(child);

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


bool Helios::IsChildOf( const SceneObject& parent, const SceneObject& possibleChild ) // NOLINT(*-no-recursion)
{
	const auto* c = TryGetComponent<ChildObject>(possibleChild);
	if (!c)
		return false; // Has no child Components, so can not be a child of parent
	if (c->parent == entt::null)
		return false; // Has no parent set, so can not be a child of parent
	if (c->parent == parent)
		return true; // Parent is the requested parent
	return IsChildOf(parent, c->parent); // Checking recursively upward
}


bool Helios::IsVisible( const SceneObject& sceneObject ) // NOLINT(*-no-recursion)
{
	for (const auto& [e] : ECS::Registry()->view<Hidden>().each())
		if (e == sceneObject)
			return false;
	if (const auto* child = TryGetComponent<ChildObject>(sceneObject);
		child != nullptr)
		return IsVisible(child->parent);
	return true;
}


void Helios::SetVisible( const SceneObject& sceneObject, bool visible )
{
	if (IsVisible(sceneObject) == visible)
		return;

	if (visible)
		ECS::Registry()->remove<Hidden>(sceneObject);
	else
		ECS::Registry()->emplace<Hidden>(sceneObject);
}


glm::mat4 Helios::GetGlobalTransform( const SceneObject& sceneObject ) // NOLINT(*-no-recursion)
{
	const auto*     child = TryGetComponent<ChildObject>(sceneObject);
	const glm::mat4 local = GetComponent<Transform>(sceneObject).GetMatrix();
	if (child == nullptr)
		return local;

	// Calling this function recursively to get global matrix
	// TODO(Quillan): Look for possible better alternative, could be overkill to do every frame if parent doesn't move
	return GetGlobalTransform(child->parent) * local;
}


SceneObject Helios::CreateSceneObject( const std::string& name )
{
	auto* reg = ECS::Registry();

	const SceneObject newObject = reg->create();
	reg->emplace<SceneObjectInfo>(newObject, name);
	reg->emplace<Transform>(newObject);
	reg->emplace<ParentObject>(newObject);
	return newObject;
}


void Helios::DestroySceneObject( const SceneObject& object )
{
	ECS::Registry()->emplace<DeleteMarker>(object);
}


void Helios::DestroyMarkedSceneObjects()
{
	for (const auto  view = ECS::Registry()->view<DeleteMarker>();
	     const auto& [e] : view.each())
		ECS::Registry()->destroy(e);
}
