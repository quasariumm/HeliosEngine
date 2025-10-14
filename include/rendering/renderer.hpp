#pragma once

#include "compute/program.hpp"
#include "compute/buffer.hpp"
#include "rendering/window.hpp"


namespace Engine
{
namespace Components
{
	struct Sphere;
	struct Material;
	struct Mesh;
	struct BVHNode;
	struct Vertex;

	struct DirectionalLight;
	struct PointLight;
	struct SpotLight;
}


class Renderer
{

public:

	void Initialize();

	void Prepare();

	void Clear() const;

	void Render() const;

	void Shutdown();

	[[nodiscard]] Window* GetWindow() const { return m_window.get(); }

private:

	std::unique_ptr<Window> m_window;

	Compute::Program m_program;

	Compute::Kernel* m_raytraceKernel  = nullptr;
	Compute::Kernel* m_intersectKernel = nullptr;


	struct GeometryContext
	{
		Components::Material* materials  = nullptr;
		Components::Sphere*   spheres    = nullptr;
		uint32_t              numSpheres = 0;
		Components::Mesh*     meshes     = nullptr;
		Components::BVHNode*  bvhNodes   = nullptr;
		Components::Vertex*   vertices   = nullptr;
		uint32_t*             indices    = nullptr;
		uint32_t              numMeshes  = 0;
	};


	struct LightsContext
	{
		Components::DirectionalLight* directionalLights    = nullptr;
		uint32_t                      numDirectionalLights = 0;
		Components::PointLight*       pointLights          = nullptr;
		uint32_t                      numPointLights       = 0;
		Components::SpotLight*        simpleSpotLights     = nullptr;
		uint32_t                      numSimpleSpotLights  = 0;
	};


	Compute::Buffer<GeometryContext> m_geometryContext{};
	Compute::Buffer<LightsContext>   m_lightsContext{};

	// Buffers for in the contexts
	Compute::Buffer<Components::Material> m_materials{Compute::BufferAccess_COPIED_READ_ONLY};
	Compute::Buffer<Components::Sphere>   m_spheres{Compute::BufferAccess_COPIED_READ_ONLY};
	Compute::Buffer<Components::Mesh>     m_meshes{Compute::BufferAccess_COPIED_READ_ONLY};
	Compute::Buffer<Components::BVHNode>  m_bvhNodes{Compute::BufferAccess_COPIED_READ_ONLY};
	Compute::Buffer<Components::Vertex>   m_vertices{Compute::BufferAccess_COPIED_READ_ONLY};
	Compute::Buffer<uint32_t>             m_indices{Compute::BufferAccess_COPIED_READ_ONLY};

	Compute::Buffer<Components::DirectionalLight> m_directionalLights{Compute::BufferAccess_COPIED_READ_ONLY};
	Compute::Buffer<Components::PointLight>       m_pointLights{Compute::BufferAccess_COPIED_READ_ONLY};
	Compute::Buffer<Components::SpotLight>        m_spotLights{Compute::BufferAccess_COPIED_READ_ONLY};
};

}
