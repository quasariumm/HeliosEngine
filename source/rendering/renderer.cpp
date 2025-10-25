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

	// m_program.LoadFromFile("shaders/opencl/raytracing/raytrace.cl", "shaders/opencl");
	//
	// m_raytraceKernel  = &m_program.GetKernel("Raytrace");
	// m_intersectKernel = &m_program.GetKernel("GetIntersectionIdx");
	//
	// // Create context buffers
	// m_geometryContext = Compute::Buffer{
	// 		new GeometryContext{},
	// 		1,
	// 		Compute::BufferAccess_COPIED_READ_ONLY
	// };
	// m_lightsContext = Compute::Buffer{
	// 		new LightsContext{},
	// 		1,
	// 		Compute::BufferAccess_COPIED_READ_ONLY
	// };
	// m_skyboxInfo = Compute::Buffer{
	// 		new SkyboxInfo{},
	// 		1,
	// 		Compute::BufferAccess_COPIED_READ_ONLY
	// };

	// Initialise texture
	const glm::uvec2 viewportSize = Systems::GetViewport()->GetViewportSize();
	m_renderTexture.FillBlank(viewportSize.x, viewportSize.y, 4, TextureFormat::RGBA32F, true);

	Systems::GetViewport()->SetRenderImage(&m_renderTexture);
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
// 	// Fill/Override the geometry context
// 	auto* geomCtx = m_geometryContext.GetData().data();
//
// 	auto AssignData = []<typename T>( Compute::Buffer<T>& buffer, T*& data, uint32_t* count = nullptr ) -> void
// 	{
// 		std::vector<T> vec;
// 		ECS::Registry()->view<T>().each([&vec]( const T& o )
// 		{
// 			vec.push_back(o);
// 		});
// 		buffer.ChangeData(vec.data(), vec.size());
// 		data = buffer.GetData().data();
// 		if (count)
// 			*count = vec.size();
// 	};
//
// 	// Materials
// 	AssignData(m_materials, geomCtx->materials);
// 	// Spheres
// 	AssignData(m_spheres, geomCtx->spheres, &geomCtx->numSpheres);
//
// 	// TODO: Meshes, BVH Nodes, Vertices, Indices
//
// 	// Fill/Override the lights context
// 	auto* lightsCtx = m_lightsContext.GetData().data();
//
// 	// Directional lights
// 	AssignData(m_directionalLights, lightsCtx->directionalLights, &lightsCtx->numDirectionalLights);
//
// 	// Point lights
// 	AssignData(m_pointLights, lightsCtx->pointLights, &lightsCtx->numPointLights);
//
// 	// // Spotlights
// 	// AssignData(m_spotLights, lightsCtx->spotLights, &lightsCtx->numSpotLights);
//
// 	// Set default skybox info
// 	auto* skyboxInfo             = m_skyboxInfo.GetData().data();
// 	skyboxInfo->groundColor      = glm::vec3(0.5, 0.5, 0.5);
// 	skyboxInfo->horizonColor     = glm::vec3(0.78, 0.87, 1.0);
// 	skyboxInfo->zenithColor      = glm::vec3(0.68, 0.84, 1.0);
// 	skyboxInfo->sunDirection     = glm::normalize(glm::vec3(1.f, -1.f, 0.f));
// 	skyboxInfo->sunFocus         = 150.f;
// 	skyboxInfo->sunIntensity     = 3.f;
// 	skyboxInfo->useSkyboxTexture = false;
//
// 	// NOTE: This is just a reference
// 	// __read_write image2d_t outImage, uint2 screenDimensions, float16 camToWorld, float16 vp, float16 prevVP, float3 viewParams, uint frame,
// 	// __read_only image2d_t skyboxTexture, __global struct SkyboxInfo* skyboxInfo,
// 	// __global struct GeometryContext* geometryContext,
// 	// __global struct LightsContext* lightsContext
//
// 	const glm::uvec2 viewportSize = Systems::GetViewport()->GetViewportSize();
// 	const glm::mat4  view         = Systems::GetCamera()->GetViewMatrix();
// 	const glm::mat4  proj         = Systems::GetCamera()->GetProjectionMatrix(viewportSize);
// 	const glm::mat4  vp           = proj * view;
// 	const glm::mat4  camToWorld   = Systems::GetCamera()->GetCamToWorldMatrix();
//
// 	// Change texture if viewport changed
// 	if (m_renderTexture.GetWidth() != viewportSize.x
// 	    || m_renderTexture.GetHeight() != viewportSize.y)
// 	{
// 		m_renderTexture.FillBlank(viewportSize.x, viewportSize.y, 4, TextureFormat::RGBA32F, true);
// 		// Reset the frame to clear accumulator since the texture has a new size thus faulty data
// 		m_frame = 0;
// 	}
//
// 	// Set out texture
// 	m_raytraceKernel->SetArguments(0, m_renderTexture.GetImage());
//
// 	// Set viewport and camera parameters
// 	m_raytraceKernel->SetArguments(1, glmToCL(viewportSize), glmToCL(camToWorld), glmToCL(vp), glmToCL(m_prevVp),
// 	                               glmToCL(Systems::GetCamera()->GetViewportParameters(viewportSize)), m_frame);
//
// 	// Set skybox info
// 	// TODO: Make filler texture object for this
// 	m_raytraceKernel->SetArguments(7, m_renderTexture.GetImage());
// 	m_raytraceKernel->SetArguments(8, *m_skyboxInfo);
//
// 	// Set contexts
// 	m_raytraceKernel->SetArguments(9, *m_geometryContext, *m_lightsContext);
//
// 	const glm::uvec2 groups = (viewportSize + 7u) >> 3u;
//
// 	std::flush(std::cout);
// 	m_raytraceKernel->Run(groups, glm::uvec2{8, 8});
//
//
// 	// Hacky. TODO: Change to separate Image2D class in Compute namespace
// 	Compute::Program::m_commandQueue.enqueueReadImage(
// 			m_renderTexture.GetImage(), CL_BLOCKING,
// 			cl::array<size_t, 3>{},
// 			cl::array<size_t, 3>{static_cast<size_t>(m_renderTexture.GetWidth()),
// 			                     static_cast<size_t>(m_renderTexture.GetHeight()), 0ull},
// 			0, 0,
// 			m_renderTexture.GetDataHDR()
// 			);
//
// #ifdef HELIOS_API_GL46
// 	m_renderTexture.UpdateData();
// #endif
//
// 	Compute::Program::Finish();
// 	m_frame++;
//
// 	m_prevVp = vp;

	m_window->SwapBuffers();
}
