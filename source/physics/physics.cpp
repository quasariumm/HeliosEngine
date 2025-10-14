#include "physics/physics.hpp"

#include "components/physics_components.hpp"

using namespace Engine;
using namespace Physics;
using namespace Components;
using namespace JPH;

/// ==============================================================================
/// Jolt implementation based on example
/// https://github.com/jrouwe/JoltPhysics/blob/master/HelloWorld/HelloWorld.cpp
/// ==============================================================================

// void PhysicsDebugRenderer::DrawLine(RVec3Arg inFrom, RVec3Arg inTo, ColorArg inColor)
// {
//     glm::vec3 p1 = {inFrom.GetX(), inFrom.GetY(), inFrom.GetZ()};
//     glm::vec3 p2 = {inTo.GetX(), inTo.GetY(), inTo.GetZ()};
//     Vec4 color = inColor.ToVec4();
//     glm::vec4 c = { color.GetX(), color.GetY(), color.GetZ(), color.GetW() };
//     // TODO(Patrick): Pls gimme line thnx :)
//     // render_debug_line(glm::value_ptr(p1), glm::value_ptr(p2), glm::value_ptr(c));
// }
//
// void PhysicsDebugRenderer::DrawTriangle(RVec3Arg inV1, RVec3Arg inV2, RVec3Arg inV3, ColorArg inColor,
//     ECastShadow inCastShadow)
// {
//
//     glm::vec3 p1 = {inV1.GetX(), inV1.GetY(), inV1.GetZ()};
//     glm::vec3 p2 = {inV2.GetX(), inV2.GetY(), inV2.GetZ()};
//     glm::vec3 p3 = {inV3.GetX(), inV3.GetY(), inV3.GetZ()};
//     Vec4 color = inColor.ToVec4();
//     glm::vec4 c = { color.GetX(), color.GetY(), color.GetZ(), color.GetW() };
//     // TODO(Patrick): Pls gimme line thnx :)
//     // render_debug_line(glm::value_ptr(p1), glm::value_ptr(p2), glm::value_ptr(c));
//     // render_debug_line(glm::value_ptr(p2), glm::value_ptr(p3), glm::value_ptr(c));
//     // render_debug_line(glm::value_ptr(p3), glm::value_ptr(p1), glm::value_ptr(c));
//
// }

PhysicsCore::PhysicsCore()
{
    RegisterDefaultAllocator();

    Factory::sInstance = new Factory();
    
    tempAllocator = std::make_unique<TempAllocatorImpl>(10 * 1024 * 1024);
    jobSystem = std::make_unique<JobSystemThreadPool>(cMaxPhysicsJobs, cMaxPhysicsBarriers, static_cast<int>(thread::hardware_concurrency()) - 1);

    RegisterTypes();
	
    physicsSystem.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, broadPhaseLayerInterface, objectVSBroadPhaseLayerFilter, objectVSObjectLayerFilter);

    physicsSystem.SetGravity({0.0f, 0.0f, -9.81f}); // Jolt defaults to -Y, but this engine uses -Z

    physicsSystem.OptimizeBroadPhase();

    // TODO(Quillan & Patrick): Debug Renderer for Jolt
    // debugRenderer = std::make_unique<PhysicsDebugRenderer>();
    // DebugRenderer::sInstance = debugRenderer.get();
    // debugRenderer->Init();
    // drawSettings.mDrawShape = true;
    //drawSettings.mDrawShapeWireframe = true;

    //ECS::AssignEngineSystem<&PhysicsSystem::DrawBodies>(&physicsSystem);
}

void PhysicsCore::InitializeObjects()
{
    auto spheres = ECS::Registry()->view<PhysicsSphere, Transform>();
    for (auto [e, s, t] : spheres.each())
    {
        if (!s.initialized) s.Initialize(e);
    }
    
    auto cubes = ECS::Registry()->view<PhysicsCube, Transform>();
    for (auto [e, c, t] : cubes.each())
    {
        if (!c.initialized) c.Initialize(e);
    }
}

void PhysicsCore::Update(float dt)
{
    physicsSystem.Update(dt, 1, tempAllocator.get(), jobSystem.get());

    auto spheres = ECS::Registry()->view<PhysicsSphere, Transform>();
    for (auto [e, s, t] : spheres.each())
    {
        if (!s.initialized) continue;
        RVec3 pos = BodyInterface().GetPosition(s.GetBodyID());
        Quat rot = BodyInterface().GetRotation(s.GetBodyID());
        t.SetPosition({pos.GetX(), pos.GetY(), pos.GetZ()});
        t.SetRotation( {rot.GetW(), rot.GetX(), rot.GetY(), rot.GetZ() });
    }

    auto cubes = ECS::Registry()->view<PhysicsCube, Transform>();
    for (auto [e, c, t] : cubes.each())
    {
        if (!c.initialized) continue;
        RVec3 pos = BodyInterface().GetPosition(c.GetBodyID());
        Quat rot = BodyInterface().GetRotation(c.GetBodyID());
        t.SetPosition({pos.GetX(), pos.GetY(), pos.GetZ()});
        t.SetRotation( {rot.GetW(), rot.GetX(), rot.GetY(), rot.GetZ() });
    }
}

void PhysicsCore::ResetObjects()
{
    BodyIDVector bodies;
    physicsSystem.GetBodies(bodies);
    for (BodyID id : bodies)
    {
        BodyInterface().RemoveBody(id);
        BodyInterface().DestroyBody(id);
    }
}

void PhysicsCore::DrawDebug()
{
    if (!drawDebug) return;

    // TODO(Quillan): Editor camera
    // entt::entity camera = Engine.Renderer().GetCamera();
    // glm::vec3 pos = GetComponent<Transform>(camera).GetPosition();
    // debugRenderer->SetCameraPos({pos.x, pos.y, pos.z});
    // DebugRenderer* test = DebugRenderer::sInstance;
    // physicsSystem.DrawBodies(drawSettings, test);
}
