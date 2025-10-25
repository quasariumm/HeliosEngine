#pragma once

#include "../../backends/opengl46_glfw/graphics/gl46_texture_2d.hpp"
#include "compute/buffer.hpp"
#include "compute/program.hpp"
#include "rendering/window.hpp"


namespace Engine {namespace Components
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

		void Prepare() const;

		void Clear() const;

		void Render();

		[[nodiscard]] Window* GetWindow() const { return m_window.get(); }

	private:

		std::unique_ptr<Window> m_window;

		// TODO(Patrick): Change to interface class
		GL46_Texture2D m_renderTexture;

		uint64_t m_frame = 0;

		glm::mat4 m_prevVp{1.f};

		Compute::Program m_program;

		Compute::Kernel* m_raytraceKernel  = nullptr;
		Compute::Kernel* m_intersectKernel = nullptr;


		struct GeometryContext
		{
			Components::Material* materials  = nullptr;
			Components::Sphere*   spheres    = nullptr;
			Components::Mesh*     meshes     = nullptr;
			Components::BVHNode*  bvhNodes   = nullptr;
			Components::Vertex*   vertices   = nullptr;
			uint32_t*             indices    = nullptr;
			uint32_t              numSpheres = 0;
			uint32_t              numMeshes  = 0;
		};


		struct LightsContext
		{
			Components::DirectionalLight* directionalLights    = nullptr;
			Components::PointLight*       pointLights          = nullptr;
			Components::SpotLight*        spotLights           = nullptr;
			uint32_t                      numDirectionalLights = 0;
			uint32_t                      numPointLights       = 0;
			uint32_t                      numSpotLights        = 0;
			uint32_t                      padding              = 0;
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
			uint8_t padding[3];
		};


		Compute::Buffer<GeometryContext> m_geometryContext{};
		Compute::Buffer<LightsContext>   m_lightsContext{};
		Compute::Buffer<SkyboxInfo>      m_skyboxInfo{};

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
