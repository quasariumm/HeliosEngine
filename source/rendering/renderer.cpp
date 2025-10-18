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

	Compute::Program::Initialize();

	m_window->SetMaximized(true);

	m_program.LoadFromFile("shaders/opencl/raytracing/raytrace.cl", "shaders/opencl");

	m_raytraceKernel  = &m_program.GetKernel("Raytrace");
	m_intersectKernel = &m_program.GetKernel("GetIntersectionIdx");

	// Create context buffers
	auto* geomCtx = new GeometryContext{};
	m_geometryContext.ChangeData(geomCtx, 1);
	delete geomCtx;
	auto* lightsCtx = new LightsContext{};
	m_lightsContext.ChangeData(lightsCtx, 1);
	delete lightsCtx;
	auto* skyboxInfo = new SkyboxInfo{};
	m_skyboxInfo.ChangeData(skyboxInfo, 1);
	delete skyboxInfo;

	// Initialise texture
	const glm::uvec2 viewportSize = Systems::GetViewport()->GetViewportSize();
	m_renderTexture.FillBlank(viewportSize.x, viewportSize.y, 4, TextureFormat::RGBA32F, true);

	Systems::GetViewport()->SetRenderImage(&m_renderTexture);

	// Set out texture and viewport size
	m_raytraceKernel->SetArguments(0, *m_renderTexture.GetImage(), glmToCL(viewportSize));
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


void Renderer::Render()
{
	// No-op if kernel is null
	if (m_raytraceKernel == nullptr)
		return;
	// Fill/Override the geometry context
	auto* geomCtx = m_geometryContext.GetData().data();

	auto AssignData = []<typename T>( Compute::Buffer<T>& buffer, cl_mem& data, uint32_t* count = nullptr ) -> void
	{
		std::vector<T> vec;
		ECS::Registry()->view<T>().each([&vec]( const T& o )
		{
			vec.push_back(o);
		});
		buffer.ChangeData(vec.data(), vec.size());
		if (!vec.empty())
			data = buffer.GetBuffer().get();
		if (count)
			*count = vec.size();
	};

	// Fill/Override the geometry context
	// Materials
	AssignData(m_materials, geomCtx->materials);
	// Spheres
	AssignData(m_spheres, geomCtx->spheres, &geomCtx->numSpheres);

	// TODO: Meshes, BVH Nodes, Vertices, Indices

	// Fill/Override the lights context
	auto* lightsCtx = m_lightsContext.GetData().data();

	// Directional lights
	AssignData(m_directionalLights, lightsCtx->directionalLights, &lightsCtx->numDirectionalLights);
	// Point lights
	AssignData(m_pointLights, lightsCtx->pointLights, &lightsCtx->numPointLights);

	// // Spotlights
	// AssignData(m_spotLights, lightsCtx->spotLights, &lightsCtx->numSpotLights);

	// Set default skybox info
	auto* skyboxInfo             = m_skyboxInfo.GetData().data();
	skyboxInfo->groundColor      = glm::vec3(0.5, 0.5, 0.5);
	skyboxInfo->horizonColor     = glm::vec3(0.78, 0.87, 1.0);
	skyboxInfo->zenithColor      = glm::vec3(0.68, 0.84, 1.0);
	skyboxInfo->sunDirection     = glm::normalize(glm::vec3(1.f, -1.f, 0.f));
	skyboxInfo->sunFocus         = 150.f;
	skyboxInfo->sunIntensity     = 3.f;
	skyboxInfo->useSkyboxTexture = false;

	// Update device pointers
	m_geometryContext.UpdateDevicePointer();
	m_lightsContext.UpdateDevicePointer();
	m_skyboxInfo.UpdateDevicePointer();

	// NOTE: This is just a reference
	// __read_write image2d_t outImage, uint2 screenDimensions, float16 camToWorld, float16 vp, float16 prevVP, float3 viewParams, uint frame,
	// __read_only image2d_t skyboxTexture, __global struct SkyboxInfo* skyboxInfo,
	// __global struct GeometryContext* geometryContext,
	// __global struct LightsContext* lightsContext

	const glm::uvec2 viewportSize = Systems::GetViewport()->GetViewportSize();
	const glm::mat4  view         = Systems::GetCamera()->GetViewMatrix();
	const glm::mat4  proj         = Systems::GetCamera()->GetProjectionMatrix(viewportSize);
	const glm::mat4  vp           = proj * view;
	const glm::mat4  camToWorld   = Systems::GetCamera()->GetCamToWorldMatrix();

	// Change texture if viewport changed
	if (m_renderTexture.GetWidth() != viewportSize.x
	    || m_renderTexture.GetHeight() != viewportSize.y)
	{
		m_renderTexture.FillBlank(viewportSize.x, viewportSize.y, 4, TextureFormat::RGBA32F, true);
		// Set out texture and viewport size
		m_raytraceKernel->SetArguments(0, *m_renderTexture.GetImage(), glmToCL(viewportSize));
		// Reset the frame to clear accumulator since the texture has a new size thus faulty data
		m_frame = 0;
	}

	// Set viewport and camera parameters
	m_raytraceKernel->SetArguments(2, glmToCL(camToWorld), glmToCL(vp), glmToCL(m_prevVP),
	                               glmToCL(Systems::GetCamera()->GetViewportParameters(viewportSize)), m_frame);

	// Set skybox info
	// TODO: Make filler texture object for this
	m_raytraceKernel->SetArguments(7, *m_renderTexture.GetImage());
	m_raytraceKernel->SetArguments(8, *m_skyboxInfo);

	// Set contexts
	m_raytraceKernel->SetArguments(9, *m_geometryContext, *m_lightsContext);

	const glm::uvec2 groups = (viewportSize + 15u) >> 4u;

	std::flush(std::cout);
	m_raytraceKernel->Run(groups, glm::uvec2{16, 16});

	m_renderTexture.GetImage().EnqueueRead();

#ifdef HELIOS_API_GL46
	m_renderTexture.Use(0, true);
#endif

	m_frame++;

	m_prevVP = vp;

	m_window->SwapBuffers();
}
