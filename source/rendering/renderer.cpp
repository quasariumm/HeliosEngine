#include "rendering/renderer.hpp"

#include "components/light_components.hpp"
#include "components/render_components.hpp"
#include "editor/editor_menus.hpp"
#include "rendering/camera.hpp"

using namespace Engine;


void Renderer::Initialize()
{
	CreateWin(
			m_window,
			glm::uvec2(1920, 1080),
			"Helios Engine",
			EngineWindowFlags_NoVsync
			);

	m_window->SetMaximized(true);

	m_shader.LoadFromFile("shaders/raytracing/raytrace.comp");

	m_geometryContext.materials.Init(BufferType::SHADER_STORAGE);

	// Initialise texture
	const glm::uvec2 viewportSize = Systems::GetViewport()->GetViewportSize();
	m_renderTexture.FillBlank(viewportSize.x, viewportSize.y, 4, TextureFormat::RGBA32F, true);

	m_shader.SetUInt("ScreenWidth", viewportSize.x);
	m_shader.SetUInt("ScreenHeight", viewportSize.y);

	Systems::GetViewport()->SetRenderImage(&m_renderTexture);

	// Initialise hit texture
	m_hitTexture.FillBlank(viewportSize.x, viewportSize.y, 1, TextureFormat::RED32UI, false);

	// Initialise skybox texture
	m_skyboxTexture.LoadFromFile("assets/qwantani_morning_puresky_4k.hdr", TextureFormat::RGB32F, true);

	// Initialise buffers
	m_geometryContext.materials.Init(BufferType::SHADER_STORAGE);
	m_geometryContext.spheres.Init(BufferType::SHADER_STORAGE);
	m_geometryContext.bvhNodes.Init(BufferType::SHADER_STORAGE);
	m_geometryContext.vertices.Init(BufferType::SHADER_STORAGE);
	m_geometryContext.indices.Init(BufferType::SHADER_STORAGE);
	m_geometryContext.meshes.Init(BufferType::SHADER_STORAGE);

	m_lightsContext.directionalLights.Init(BufferType::SHADER_STORAGE);
	m_lightsContext.pointLights.Init(BufferType::SHADER_STORAGE);
	m_lightsContext.spotLights.Init(BufferType::SHADER_STORAGE);
}


void Renderer::Prepare() const
{
	m_window->BeginFrame();
	m_window->PollEvents();
}


void Renderer::Clear() const
{
	m_window->ClearViewport();
}


template<typename Comp, typename GPU>
void AssignData( const GL46_Buffer& buffer, uint32_t* count = nullptr )
{
	std::vector<GPU> vec;
	ECS::Registry()->view<Comp>().each([&vec]( entt::entity e, const Comp& o )
	{
		vec.push_back(o.MakeGPU(e));
	});
	buffer.Fill(vec.size() * sizeof(GPU), vec.data());
	if (count)
		*count = vec.size();
};


void Renderer::Render()
{
	// Fill/Override the geometry data
	// // Materials
	// AssignData<Components::Material>(m_geometryContext.materials, &m_geometryContext.numMaterials);
	// Spheres
	AssignData<Components::Sphere, Components::SphereGPU>(m_geometryContext.spheres, &m_geometryContext.numSpheres);

	// TODO: Meshes, BVH Nodes, Vertices, Indices

	// // Fill/Override the lights context
	// // Directional lights
	// AssignData<Components::DirectionalLight>(m_lightsContext.directionalLights, &m_lightsContext.numDirectionalLights);
	//
	// // Point lights
	// AssignData<Components::PointLight>(m_lightsContext.pointLights, &m_lightsContext.numPointLights);
	//
	// // Spotlights
	// AssignData<Components::SpotLight>(m_lightsContext.spotLights, &m_lightsContext.numSpotLights);

	// Set default skybox info
	m_skyboxInfo.groundColor      = glm::vec3(0.5, 0.5, 0.5);
	m_skyboxInfo.horizonColor     = glm::vec3(0.78, 0.87, 1.0);
	m_skyboxInfo.zenithColor      = glm::vec3(0.68, 0.84, 1.0);
	m_skyboxInfo.sunDirection     = glm::normalize(glm::vec3(1.f, -1.f, 0.f));
	m_skyboxInfo.sunFocus         = 150.f;
	m_skyboxInfo.sunIntensity     = 3.f;
	m_skyboxInfo.useSkyboxTexture = true;

	const glm::uvec2 viewportSize = Systems::GetViewport()->GetViewportSize();

	m_shader.Use();
	// Change texture if viewport changed
	if (m_renderTexture.GetWidth() != viewportSize.x
	    || m_renderTexture.GetHeight() != viewportSize.y)
	{
		m_renderTexture.FillBlank(viewportSize.x, viewportSize.y, 4, TextureFormat::RGBA32F, true);
		m_hitTexture.FillBlank(viewportSize.x, viewportSize.y, 1, TextureFormat::RED32UI, false);
		m_shader.SetUInt("ScreenWidth", viewportSize.x);
		m_shader.SetUInt("ScreenHeight", viewportSize.y);
		// Reset the frame to clear accumulator since the texture has a new size thus faulty data
		m_frame = 0;
	}

	SendData();

	const glm::uvec2 groups = (viewportSize + 7u) >> 3u;

	m_shader.Dispatch({groups, 1});

	m_frame++;

	m_window->SwapBuffers();
}


entt::entity Renderer::GetEntityAtCursor( const glm::uvec2 position ) const
{
	return *m_hitTexture.GetPixel<entt::entity>(position);
}


void Renderer::SendData()
{
	// Bind textures
	m_renderTexture.UseCompute(0);
	m_hitTexture.UseCompute(1);
	m_skyboxTexture.UseCompute(31);

	// Bind general data
	const glm::uvec2 viewportSize = Systems::GetViewport()->GetViewportSize();
	const glm::mat4  view         = Systems::GetCamera()->GetViewMatrix();
	const glm::mat4  proj         = Systems::GetCamera()->GetProjectionMatrix(viewportSize);
	const glm::mat4  vp           = proj * view;

	m_shader.SetMat4("CamToWorld", Systems::GetCamera()->GetCamToWorldMatrix());
	m_shader.SetMat4("VPMat", vp);
	m_shader.SetMat4("PrevVPMat", m_prevVp);
	m_shader.SetVec3("ViewParams", Systems::GetCamera()->GetViewportParameters(viewportSize));

	m_shader.SetUInt("Frame", m_frame);

	m_shader.SetBool("ClearAccumulator", m_window->GetKey(Key::Q) == 1);

	// Bind geometry context
	m_geometryContext.materials.BindBase(8);
	m_geometryContext.spheres.BindBase(3);
	m_shader.SetUInt("NumSpheres", m_geometryContext.numSpheres);
	m_geometryContext.bvhNodes.BindBase(4);
	m_shader.SetUInt("NumBVHNodes", m_geometryContext.numBVHNodes);
	m_geometryContext.vertices.BindBase(5);
	m_shader.SetUInt("NumVertices", m_geometryContext.numVertices);
	m_geometryContext.indices.BindBase(6);
	m_shader.SetUInt("NumIndices", m_geometryContext.numIndices);
	m_geometryContext.meshes.BindBase(7);
	m_shader.SetUInt("NumMeshes", m_geometryContext.numMeshes);

	// Bind lights context
	m_lightsContext.directionalLights.BindBase(0);
	m_shader.SetUInt("NumDirectionalLights", m_lightsContext.numDirectionalLights);
	m_lightsContext.pointLights.BindBase(1);
	m_shader.SetUInt("NumPointLights", m_lightsContext.numPointLights);
	m_lightsContext.spotLights.BindBase(2);
	m_shader.SetUInt("NumSpotLigts", m_lightsContext.numSpotLights);

	// Bind skybox info
	m_shader.SetVec3("GroundColor", m_skyboxInfo.groundColor);
	m_shader.SetVec3("HorizonColor", m_skyboxInfo.horizonColor);
	m_shader.SetVec3("ZenithColor", m_skyboxInfo.zenithColor);
	m_shader.SetVec3("SunDirection", m_skyboxInfo.sunDirection);
	m_shader.SetFloat("SunFocus", m_skyboxInfo.sunFocus);
	m_shader.SetFloat("SunIntensity", m_skyboxInfo.sunIntensity);
	m_shader.SetBool("UseSkyboxTexture", m_skyboxInfo.useSkyboxTexture);

	m_prevVp = vp;
}
