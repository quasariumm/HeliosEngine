#include "components/physics_components.hpp"

#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

using namespace Engine;
using namespace Components;

void PhysicsComponent::Initialize(entt::entity owner)
{
    if (initialized)
    {
        Log::Warn("Trying to initialize component multiple times");
        return;
    }

    transform = &GetComponent<Transform>(owner);
    transform->SubscribeOnModify<&PhysicsComponent::OnTransformModified>(this);
    
    initialized = true;
}

void PhysicsComponent::OnTransformModified() const
{
    if (!initialized)
    {
        Log::Error("Transform was modified before physics component was initialized!");
        return;
    }
    
    if (!transform)
    {
        Log::Error("Physics Component was initialized without calling initialize on the base component!");
        return;
    }

    glm::vec3 pos = transform->GetPosition();
    glm::quat rot = transform->GetRotation();

    Systems::GetPhysics()->BodyInterface().SetPositionAndRotation(bodyID,
        {pos.x, pos.y, pos.z},
        {rot.w, rot.x, rot.y, rot.z},
        EActivation::Activate);
}

void PhysicsComponent::Inspector()
{
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_Button));
    ImVec2 area = { ImGui::GetContentRegionAvail().x, ImGui::GetFontSize() * 1.5f };
    if (ImGui::BeginChild("Movement selector", area))
    {
        ImVec2     size     = { area.x / 3.0f, area.y };
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2( 0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2( 0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, {0.5f, 0.5f});
        if (ImGui::Selectable("Static", motionType == EMotionType::Static, ImGuiSelectableFlags_None, size))
        {
            motionType = EMotionType::Static;
            Systems::GetPhysics()->BodyInterface().SetMotionType(bodyID, motionType, EActivation::Activate);
        }
        ImGui::SameLine();
        if (ImGui::Selectable("Dynamic", motionType == EMotionType::Dynamic, ImGuiSelectableFlags_None, size))
        {
            motionType = EMotionType::Dynamic;
            Systems::GetPhysics()->BodyInterface().SetMotionType(bodyID, motionType, EActivation::Activate);
        }
        ImGui::SameLine();
        if (ImGui::Selectable("Kinematic", motionType == EMotionType::Kinematic, ImGuiSelectableFlags_None, size))
        {
            motionType = EMotionType::Kinematic;
            Systems::GetPhysics()->BodyInterface().SetMotionType(bodyID, motionType, EActivation::Activate);
        }
        ImGui::PopStyleVar(3);
        ImGui::EndChild();
    }
    ImGui::PopStyleColor();
}

void PhysicsSphere::Initialize(entt::entity owner)
{
    PhysicsComponent::Initialize(owner);

    shape = new SphereShape(size);
    BodyCreationSettings sphereSettings{ shape, {}, Quat::sIdentity(), motionType, Layers::MOVING };
    sphereSettings.mAllowDynamicOrKinematic = true;
    bodyID = Systems::GetPhysics()->BodyInterface().CreateAndAddBody(sphereSettings, EActivation::Activate);

    OnTransformModified();
}

void PhysicsCube::Initialize(entt::entity owner)
{
    PhysicsComponent::Initialize(owner);

    shape = new BoxShape({size.x, size.y, size.z});
    BodyCreationSettings cubeSettings { shape, {}, Quat::sIdentity(), motionType, Layers::MOVING };
    cubeSettings.mAllowDynamicOrKinematic = true;
    bodyID = Systems::GetPhysics()->BodyInterface().CreateAndAddBody(cubeSettings, EActivation::Activate);

    OnTransformModified();
}
