#include "compute/compute_program.hpp"

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

	m_context = cl::Context{m_device};

	m_commandQueue = cl::CommandQueue{m_context, m_device};
}


cl::Program& Program::LoadFromFile( const std::filesystem::path& path )
{
	std::ifstream file;
	file.open(path);

	if (!file.is_open())
	{
		Log::Error(std::format("Failed to open file: {}", path.string()));
		return m_program;
	}

	const auto programSource = std::string{
			std::istreambuf_iterator<char>(file),
			std::istreambuf_iterator<char>()
	};

	const cl::Program::Sources sources{1, programSource};

	m_program = cl::Program{m_context, sources};

	if (const auto err = m_program.build(m_device, "-cl-std=CL" CL_VERSION_STR);
		err != CL_SUCCESS)
		Log::Error(std::format("Failed to build compute program {}. Error: {}", path.string(), CLErrorString(err)));

	return m_program;
}


Kernel& Program::GetKernel( const std::string& name )
{
	if (m_loadedKernels.contains(name))
		return m_loadedKernels.at(name);

	return m_loadedKernels.emplace(name, Kernel{cl::Kernel{m_program, name}}).first->second;
}
