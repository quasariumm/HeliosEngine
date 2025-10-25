#pragma once

namespace Engine::Physics
{
using namespace JPH;


namespace Layers
{
	static constexpr ObjectLayer NON_MOVING = 0;
	static constexpr ObjectLayer MOVING     = 1;
	static constexpr ObjectLayer NUM_LAYERS = 2;
};


/// Class that determines if two object layers can collide
class ObjectLayerPairFilterImpl final : public ObjectLayerPairFilter
{
public:

	[[nodiscard]]
	bool ShouldCollide( const ObjectLayer inObject1, const ObjectLayer inObject2 ) const override
	{
		switch (inObject1)
		{
		case Layers::NON_MOVING:
			return inObject2 == Layers::MOVING; // Non-moving only collides with moving
		case Layers::MOVING:
			return true; // Moving collides with everything
		default:
			JPH_ASSERT(false);
			return false;
		}
	}
};


namespace BroadPhaseLayers
{
	static constexpr BroadPhaseLayer NON_MOVING(0);
	static constexpr BroadPhaseLayer MOVING(1);
	static constexpr uint            NUM_LAYERS(2);
};


// BroadPhaseLayerInterface implementation
// This defines a mapping between object and broad-phase layers.
class BPLayerInterfaceImpl final : public BroadPhaseLayerInterface
{
public:

	BPLayerInterfaceImpl()
	{
		// Create a mapping table from object to broad phase layer
		mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
		mObjectToBroadPhase[Layers::MOVING]     = BroadPhaseLayers::MOVING;
	}


	[[nodiscard]]
	uint GetNumBroadPhaseLayers() const override
	{
		return BroadPhaseLayers::NUM_LAYERS;
	}


	[[nodiscard]]
	BroadPhaseLayer GetBroadPhaseLayer( const ObjectLayer inLayer ) const override
	{
		JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
		return mObjectToBroadPhase[inLayer];
	}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
	[[nodiscard]]
	const char* GetBroadPhaseLayerName( const BroadPhaseLayer inLayer ) const override
	{
		switch (static_cast<BroadPhaseLayer::Type>(inLayer))
		{
		case static_cast<BroadPhaseLayer::Type>(BroadPhaseLayers::NON_MOVING):
			return "NON_MOVING";
		case static_cast<BroadPhaseLayer::Type>(BroadPhaseLayers::MOVING):
			return "MOVING";
		default: JPH_ASSERT(false);
			return "INVALID";
		}
	}
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

private:

	BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
};


/// Class that determines if an object layer can collide with a broad-phase layer
class ObjectVsBroadPhaseLayerFilterImpl final : public ObjectVsBroadPhaseLayerFilter
{
public:

	[[nodiscard]]
	bool ShouldCollide( const ObjectLayer inLayer1, const BroadPhaseLayer inLayer2 ) const override
	{
		switch (inLayer1)
		{
		case Layers::NON_MOVING:
			return inLayer2 == BroadPhaseLayers::MOVING;
		case Layers::MOVING:
			return true;
		default:
			JPH_ASSERT(false);
			return false;
		}
	}
};


// class PhysicsDebugRenderer : public DebugRendererSimple
// {
// public:
// 	PhysicsDebugRenderer() = default;
// 	void Init() { Initialize(); }
//
// 	void DrawLine(RVec3Arg inFrom, RVec3Arg inTo, ColorArg inColor) override;
// 	void DrawTriangle(RVec3Arg inV1, RVec3Arg inV2, RVec3Arg inV3, ColorArg inColor, ECastShadow inCastShadow) override;
// 	void DrawText3D(RVec3Arg inPosition, const string_view& inString, ColorArg inColor, float inHeight) override {}
// 	// void DrawGeometry(RMat44Arg inModelMatrix, const AABox& inWorldSpaceBounds, float inLODScaleSq, ColorArg inModelColor, const GeometryRef& inGeometry, ECullMode inCullMode, ECastShadow inCastShadow, EDrawMode inDrawMode) override {}
// 	// Batch CreateTriangleBatch(const DebugRenderer::Vertex* inVertices, int inVertexCount, const uint32* inIndices, int inIndexCount) override { return Batch(); }
// 	// Batch CreateTriangleBatch(const Triangle* inTriangles, int inTriangleCount) override { return Batch(); }
// };

class PhysicsCore
{
public:

	PhysicsCore();

	static void InitializeObjects();

	void Update( float dt );

	void ResetObjects();

	void DrawDebug();

	BodyInterface& BodyInterface() { return physicsSystem.GetBodyInterface(); }

	bool drawDebug = true;

private:

	BodyManager::DrawSettings drawSettings;

	const uint cMaxBodies             = 10000;
	const uint cNumBodyMutexes        = 0;
	const uint cMaxBodyPairs          = 10000;
	const uint cMaxContactConstraints = 10000;

	std::unique_ptr<TempAllocatorImpl>   tempAllocator; // Allocating 10 MB
	std::unique_ptr<JobSystemThreadPool> jobSystem;

	BPLayerInterfaceImpl              broadPhaseLayerInterface;
	ObjectVsBroadPhaseLayerFilterImpl objectVSBroadPhaseLayerFilter;
	ObjectLayerPairFilterImpl         objectVSObjectLayerFilter;

	//std::unique_ptr<PhysicsDebugRenderer> debugRenderer;

	PhysicsSystem physicsSystem;
};
}
