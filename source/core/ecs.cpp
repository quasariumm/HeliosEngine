#include "core/ecs.hpp"

#include "entt/entity/registry.hpp"
#include "entt/entity/snapshot.hpp"

#include "components/basic_components.hpp"

using namespace Engine;
using namespace Components;


void EntityComponentSystem::SaveSnapshot()
{
	nlohmann::json output;

	auto view = m_registry.view<SceneObjectInfo>();
	for (auto [e, info] : view.each())
	{
		Serialization::Serializer serializer;
		for (const auto& component : serializeComponents)
			component(e, serializer);

		std::string objName = std::to_string(static_cast<uint32_t>(e));
		output[objName] = serializer.GetBuffer();
	}

	m_snapshot = output;
}


void EntityComponentSystem::LoadSnapshot()
{
	//TODO(Quillan): Load all entities and components

	entt::registry temp;

	for (nlohmann::json entityData : m_snapshot)
	{
		entt::entity e = m_registry.create();
		Serialization::Serializer serializer = { entityData };

		for (const auto& component : deserializeComponents)
			component(e, serializer, temp);
	}

	m_registry.swap(temp);

	// m_snapshot.clear();
	// m_snapshot.seekg(0);
	//
	// cereal::JSONInputArchive input{m_snapshot};
	// entt::registry           temp;
	//
	// auto l = entt::snapshot_loader(temp);
	//
	// for (const auto& c : serializedComponentsInput)
	// 	c(l, input);
	//
	// // Reparent because parent objects are not saved
	// for (const auto  objects = temp.view<SceneObjectInfo>();
	//      const auto& [e, s] : objects.each())
	// 	temp.emplace<ParentObject>(e);
	//
	// for (const auto  children = temp.view<ChildObject>();
	//      const auto& [e, c] : children.each())
	// {
	// 	ParentObject* p = &temp.get<ParentObject>(c.parent);
	// 	p->AddChild(e);
	// }
	//
	// m_registry.swap(temp);
}
