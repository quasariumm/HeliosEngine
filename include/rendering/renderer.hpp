#pragma once

#include "../../backends/opengl46_glfw/graphics/gl46_texture_2d.hpp"
#include "compute/program.hpp"
#include "compute/buffer.hpp"
#include "rendering/window.hpp"


namespace Engine::Components
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


namespace Engine
{
class Renderer
{
public:

	void Initialize();

	void Prepare() const;

	void Clear() const;

	void Render();

	[[nodiscard]] Window* GetWindow() const { return m_window.get(); }

private:

	std::unique_ptr<Window> m_window;

	// TODO(Patrick): Change to interface class
	GL46_Texture2D m_renderTexture;

	uint64_t m_frame = 0;

	glm::mat4 m_prevVP{1.f};

	Compute::Program m_program;

	Compute::Kernel* m_raytraceKernel  = nullptr;
	Compute::Kernel* m_intersectKernel = nullptr;


	struct GeometryContext
	{
		cl_mem   materials  = nullptr;
		cl_mem   spheres    = nullptr;
		cl_mem   meshes     = nullptr;
		cl_mem   bvhNodes   = nullptr;
		cl_mem   vertices   = nullptr;
		cl_mem   indices    = nullptr;
		uint32_t numSpheres = 0;
		uint32_t numMeshes  = 0;
	};


	struct LightsContext
	{
		cl_mem   directionalLights    = nullptr;
		cl_mem   pointLights          = nullptr;
		cl_mem   spotLights           = nullptr;
		uint32_t numDirectionalLights = 0;
		uint32_t numPointLights       = 0;
		uint32_t numSpotLights        = 0;
		uint32_t _padding             = 0;
	};


	struct SkyboxInfo
	{
		glm::vec3 groundColor;
		glm::vec3 horizonColor;
		glm::vec3 zenithColor;
		glm::vec3 sunDirection;

		float sunFocus;
		float sunIntensity;

		bool    useSkyboxTexture;
		uint8_t _padding[3]{0};
	};


	Compute::Buffer<GeometryContext> m_geometryContext{Compute::Access_ALLOCED_READ_ONLY};
	Compute::Buffer<LightsContext>   m_lightsContext{Compute::Access_ALLOCED_READ_ONLY};
	Compute::Buffer<SkyboxInfo>      m_skyboxInfo{Compute::Access_ALLOCED_READ_ONLY};

	// Buffers for in the contexts
	Compute::Buffer<Components::Material> m_materials{Compute::Access_ALLOCED_READ_ONLY};
	Compute::Buffer<Components::Sphere>   m_spheres{Compute::Access_ALLOCED_READ_ONLY};
	Compute::Buffer<Components::Mesh>     m_meshes{Compute::Access_ALLOCED_READ_ONLY};
	Compute::Buffer<Components::BVHNode>  m_bvhNodes{Compute::Access_ALLOCED_READ_ONLY};
	Compute::Buffer<Components::Vertex>   m_vertices{Compute::Access_ALLOCED_READ_ONLY};
	Compute::Buffer<uint32_t>             m_indices{Compute::Access_ALLOCED_READ_ONLY};

	Compute::Buffer<Components::DirectionalLight> m_directionalLights{Compute::Access_ALLOCED_READ_ONLY};
	Compute::Buffer<Components::PointLight>       m_pointLights{Compute::Access_ALLOCED_READ_ONLY};
	Compute::Buffer<Components::SpotLight>        m_spotLights{Compute::Access_ALLOCED_READ_ONLY};
};
}
