#include "core/ecs.hpp"

#include "entt/entity/registry.hpp"

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

		std::string objName = info.name + "_" +  std::to_string(static_cast<uint32_t>(e));
		output[objName] = serializer.GetBuffer();
	}

	m_snapshot = output;
}


void EntityComponentSystem::LoadSnapshot()
{
	entt::registry temp;

	for (nlohmann::json entityData : m_snapshot)
	{
		entt::entity e = temp.create();
		Serialization::Serializer serializer = { entityData };

		for (const auto& component : deserializeComponents)
			component(e, serializer, temp);
	}

	m_registry.swap(temp);
}
