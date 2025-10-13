#include "core/ecs.hpp"

#include "cereal/archives/json.hpp"

#include "entt/entity/snapshot.hpp"
#include "entt/entity/registry.hpp"

#include "components/basic_components.hpp"

using namespace Engine;
using namespace entt;
using namespace Components;

void EntityComponentSystem::SaveSnapshot()
{
    snapshot.str("");
    snapshot.clear();
    cereal::JSONOutputArchive output{ snapshot };
    
    auto tempSnapshot = entt::snapshot(*Registry());
    
    for (const auto& component : serializedComponentsOutput)
        component(tempSnapshot, output);
}

void EntityComponentSystem::LoadSnapshot()
{
    snapshot.clear();
    snapshot.seekg(0);
    
    cereal::JSONInputArchive input{ snapshot };
    entt::registry temp;
    
    auto l = entt::snapshot_loader (temp);
    
    for (const auto& c : serializedComponentsInput)
        c(l, input);
    
    // Reparent because parent objects are not saved
    auto objects = temp.view<SceneObjectInfo>();
    for (const auto& [e, s] : objects.each())
        temp.emplace<ParentObject>(e);
    
    auto children = temp.view<ChildObject>();
    for (const auto& [e, c] : children.each())
    {
        ParentObject* p = &temp.get<ParentObject>(c.parent);
        p->AddChild(e);
    }

    registry.swap(temp);
}
