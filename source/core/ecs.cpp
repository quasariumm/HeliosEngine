#include "core/ecs.hpp"

#include "cereal/archives/json.hpp"

#include "entt/entity/registry.hpp"
#include "entt/entity/snapshot.hpp"

#include "components/basic_components.hpp"

using namespace Engine;
using namespace Components;


void EntityComponentSystem::SaveSnapshot()
{
	m_snapshot.str("");
	m_snapshot.clear();
	cereal::JSONOutputArchive output{m_snapshot};

	auto tempSnapshot = entt::snapshot(*Registry());

	for (const auto& component : serializedComponentsOutput)
		component(tempSnapshot, output);
}


void EntityComponentSystem::LoadSnapshot()
{
	m_snapshot.clear();
	m_snapshot.seekg(0);

	cereal::JSONInputArchive input{m_snapshot};
	entt::registry           temp;

	auto l = entt::snapshot_loader(temp);

	for (const auto& c : serializedComponentsInput)
		c(l, input);

	// Reparent because parent objects are not saved
	for (const auto  objects = temp.view<SceneObjectInfo>();
	     const auto& [e, s] : objects.each())
		temp.emplace<ParentObject>(e);

	for (const auto  children = temp.view<ChildObject>();
	     const auto& [e, c] : children.each())
	{
		ParentObject* p = &temp.get<ParentObject>(c.parent);
		p->AddChild(e);
	}

	m_registry.swap(temp);
}
