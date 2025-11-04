#pragma once

#include "../../backends/opengl46_glfw/core/gl46_buffer.hpp"
#include "../../backends/opengl46_glfw/graphics/gl46_compute_shader.hpp"
#include "../../backends/opengl46_glfw/graphics/gl46_texture_2d.hpp"
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

	[[nodiscard]]
	Window* GetWindow() const { return m_window.get(); }

	[[nodiscard]]
	entt::entity GetEntityAtCursor(glm::uvec2 position) const;

private:

	void SendData();

	std::unique_ptr<Window> m_window;

	// TODO(Patrick): Change to interface class
	GL46_Texture2D m_renderTexture;
	GL46_Texture2D m_hitTexture;
	GL46_Texture2D m_skyboxTexture;

	uint64_t m_frame = 0;

	glm::mat4 m_prevVp{1.f};

	GL46_ComputeShader m_shader;


	struct GeometryContext
	{
		GL46_Buffer materials;
		GL46_Buffer spheres;
		GL46_Buffer meshes;
		GL46_Buffer bvhNodes;
		GL46_Buffer vertices;
		GL46_Buffer indices;
		uint32_t    numMaterials = 0;
		uint32_t    numSpheres = 0;
		uint32_t    numMeshes = 0;
		uint32_t    numBVHNodes = 0;
		uint32_t    numVertices = 0;
		uint32_t    numIndices = 0;
	} m_geometryContext{};


	struct LightsContext
	{
		GL46_Buffer directionalLights;
		GL46_Buffer pointLights;
		GL46_Buffer spotLights;
		uint32_t    numDirectionalLights = 0;
		uint32_t    numPointLights       = 0;
		uint32_t    numSpotLights        = 0;
		uint32_t    padding              = 0;
	} m_lightsContext{};


	struct SkyboxInfo
	{
		glm::vec3 groundColor;
		glm::vec3 horizonColor;
		glm::vec3 zenithColor;
		glm::vec3 sunDirection;

		float sunFocus;
		float sunIntensity;

		bool useSkyboxTexture;
	} m_skyboxInfo{};
};
}
