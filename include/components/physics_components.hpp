#pragma once
#include <cereal/cereal.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "basic_components.hpp"
#include "physics/physics.hpp"

#include "core/ecs.hpp"

namespace Engine::Components
{
using namespace JPH;
using namespace Physics;

struct PhysicsComponent : BaseComponent
{
    bool initialized = false;

    virtual void Initialize(entt::entity owner);

    BodyID GetBodyID() const { return bodyID; }
    
    EMotionType motionType = EMotionType::Dynamic;
    
protected:
    BodyID bodyID;
    
    Transform* transform = nullptr;

    template<typename Archive>
    void serialize(Archive &archive)
    {
        uint8 value = static_cast<uint8>(motionType);
        archive(CEREAL_NVP(value)); // serialize the byte form

        if constexpr (Archive::is_loading::value)
        {
            motionType = static_cast<EMotionType>(value);
        }
    }
    
    void OnTransformModified() const;

    void Inspector() override;

};

struct PhysicsSphere : PhysicsComponent
{
    PhysicsSphere() = default;

    float size = 1.0f;

    SphereShape* shape = nullptr;
    
    void Initialize(entt::entity owner) override;

    void Inspector() override
    {
        PhysicsComponent::Inspector();
        if (ImGui::InputFloat("Radius", &size))
            shape->ScaleShape({size, size, size});
    }

    
    template<typename Archive>
    void serialize(Archive &archive) { PhysicsComponent::serialize(archive); archive(CEREAL_NVP(size)); }
};

REGISTER_COMPONENT(PhysicsSphere, ICON_SPHERE" Physics Sphere", ADDABLE | INSPECTABLE)

struct PhysicsCube : PhysicsComponent
{
    PhysicsCube() = default;

    glm::vec3 size = { 1.0f, 1.0f, 1.0f };

    BoxShape* shape = nullptr;

    void Initialize(entt::entity owner) override;
    
    void Inspector() override
    {
        PhysicsComponent::Inspector();
        if (ImGui::InputFloat3("Size", glm::value_ptr(size)))
            shape->ScaleShape({size.x, size.y, size.z});
    }
    
    template<typename Archive>
    void serialize(Archive &archive) { PhysicsComponent::serialize(archive); archive(CEREAL_NVP(size)); }
};

REGISTER_COMPONENT(PhysicsCube, ICON_CUBE" Physics Cube", ADDABLE | INSPECTABLE)

}
