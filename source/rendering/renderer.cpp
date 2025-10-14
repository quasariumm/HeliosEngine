#include "rendering/renderer.hpp"

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

	m_program.LoadFromFile("shaders/opencl/raytracing/raytrace.cl", "shaders/opencl");

	m_raytraceKernel = &m_program.GetKernel("Raytrace");
	m_intersectKernel = &m_program.GetKernel("GetIntersectionIdx");

	// Create context buffers
	m_geometryContext = Compute::Buffer{
		new GeometryContext{},
		1,
		Compute::BufferAccess_COPIED_READ_ONLY
	};
	m_lightsContext = Compute::Buffer{
		new LightsContext{},
		1,
		Compute::BufferAccess_COPIED_READ_ONLY
	};
}

void Renderer::Prepare()
{
    m_window.get()->BeginFrame();
    m_window.get()->PollEvents();

	// Fill/Override the context buffers TODO:

	auto* geomCtx = m_geometryContext.GetData().data();
}

void Renderer::Clear() const
{
    m_window.get()->ClearViewport();
}

void Renderer::Render() const
{
    m_window.get()->SwapBuffers();

	// m_raytraceKernel->SetArguments(0, )
}


void Renderer::Shutdown()
{
	delete m_geometryContext.GetData().data();
}
