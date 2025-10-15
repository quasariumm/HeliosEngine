#include "compute/program.hpp"

#ifdef HELIOS_API_GL46
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>
#endif

#include <CL/Utils/Utils.hpp>

#include "debugging/logger.hpp"

using namespace Engine::Compute;

cl::Platform     Program::m_platform{};
cl::Device       Program::m_device{};
cl::Context      Program::m_context{};
cl::CommandQueue Program::m_commandQueue{};


void Program::Initialize()
{
	cl_int err;
	m_platform = cl::Platform::getDefault(&err);

	if (err != CL_SUCCESS)
		Log::Error(std::format("Getting default CL platform failed. Error: {}", CLErrorString(err)));

	std::vector<cl::Device> devices;
	err = m_platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);

	if (err != CL_SUCCESS)
		Log::Error(std::format("Getting devices failed. Error: {}", CLErrorString(err)));

	m_device = devices[0];
	Log::Info(std::format("Using GPU: {} (OpenCL Version: {})", m_device.getInfo<CL_DEVICE_NAME>(),
	                      m_device.getInfo<CL_DEVICE_VERSION>()));

#ifdef HELIOS_API_GL46
	GLFWwindow* window = glfwGetCurrentContext();
	cl_context_properties props[] = {
		CL_GL_CONTEXT_KHR, (cl_context_properties)glfwGetWGLContext(window),
		CL_WGL_HDC_KHR, (cl_context_properties)GetDC(glfwGetWin32Window(window)),
		CL_CONTEXT_PLATFORM, (cl_context_properties)m_platform(),
		0
	};
	m_context = cl::Context{m_device, props, nullptr, nullptr, &err};
	if (err != CL_SUCCESS)
		Log::Error(std::format("Failed to create context. Error: {}", CLErrorString(err)));
#else
	m_context = cl::Context{m_device};
#endif

	m_commandQueue = cl::CommandQueue{m_context, m_device};
}


cl::Program& Program::LoadFromFile( const std::filesystem::path& path, const std::string& includeDir, const std::vector<std::string>& includeSources )
{
	cl::Program::Sources sources{1, LoadFile(path)};
	sources.insert(sources.begin(), includeSources.begin(), includeSources.end());

	m_program = cl::Program{m_context, sources};

	std::string buildOptions = "-g -cl-kernel-arg-info -cl-std=CL" CL_VERSION_STR;

	if (!includeDir.empty())
		buildOptions += " -I " + includeDir;

	if (const auto err = m_program.build(m_device, buildOptions.c_str());
		err != CL_SUCCESS)
		Log::Error(std::format("Failed to build compute program {}. See log below. CL error: {}", path.string(), CLErrorString(err)));
	// Get the size of the build log
	size_t log_size;
	clGetProgramBuildInfo(m_program.get(), m_device.get(), CL_PROGRAM_BUILD_LOG,
						  0, NULL, &log_size);

	// Allocate memory for the log
	auto *log = static_cast<char*>(malloc(log_size));

	// Get the actual build log
	clGetProgramBuildInfo(m_program.get(), m_device.get(), CL_PROGRAM_BUILD_LOG,
						  log_size, log, NULL);

	Log::Info(std::format("Build log:\n{}\n", log));
	free(log);

	return m_program;
}


void Program::Finish() const
{
	cl_int err = m_commandQueue.finish();
	if (err != CL_SUCCESS)
		Log::Error(std::format("Failed to finish command queue. Error: {}", CLErrorString(err)));
}


Kernel& Program::GetKernel( const std::string& name )
{
	if (m_loadedKernels.contains(name))
		return m_loadedKernels.at(name);

	return m_loadedKernels.emplace(name, Kernel{cl::Kernel{m_program, name}}).first->second;
}
