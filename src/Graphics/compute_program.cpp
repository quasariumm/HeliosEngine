#include "compute_program.hpp"

#include <CL/Utils/Utils.hpp>

#include "Debugger/Debugger.h"

using namespace Engine::Graphics;

cl::Platform     ComputeProgram::m_platform{};
cl::Device       ComputeProgram::m_device{};
cl::Context      ComputeProgram::m_context{};
cl::CommandQueue ComputeProgram::m_commandQueue{};


void ComputeProgram::Initialize()
{
	cl_int err;
	m_platform = cl::Platform::getDefault(&err);

	if (err != CL_SUCCESS)
		DebugLog(LogSeverity::SEVERE,
		         std::format(L"Getting default CL platform failed. Error: {}", CLErrorString(err)));

	std::vector<cl::Device> devices;
	err = m_platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);

	if (err != CL_SUCCESS)
		DebugLog(LogSeverity::SEVERE, std::format(L"Getting devices failed. Error: {}", CLErrorString(err)));

	m_device = devices[0];
	DebugLog(LogSeverity::INFO,
	         STR_TO_WSTR(
			         std::format("Using GPU: {} (OpenCL Version: {})", m_device.getInfo<CL_DEVICE_NAME>(), m_device.
				         getInfo<
				         CL_DEVICE_VERSION>())));

	m_context = cl::Context{m_device};

	m_commandQueue = cl::CommandQueue{m_context, m_device};
}


cl::Program& ComputeProgram::LoadFromFile( const std::filesystem::path& path )
{
	std::ifstream file;
	file.open(path);

	if (!file.is_open())
	{
		DebugLog(LogSeverity::SEVERE, std::format(L"Failed to open file: {}", path.wstring()));
		return m_program;
	}

	const auto programSource = std::string{
			std::istreambuf_iterator<char>(file),
			std::istreambuf_iterator<char>()
	};

	const cl::Program::Sources sources{1, programSource};

	m_program = cl::Program{m_context, sources};

	if (const auto err = m_program.build(m_device, std::format("-cl-std={}", CL_VERSION_STR));
		err != CL_SUCCESS)
		DebugLog(LogSeverity::SEVERE,
		         std::format(L"Failed to build compute program {}. Error: {}", path.wstring(), CLErrorString(err)));

	return m_program;
}


ComputeKernel& ComputeProgram::GetKernel( const std::string& name )
{
	if (m_loadedKernels.contains(name))
		return m_loadedKernels.at(name);

	return m_loadedKernels.emplace(std::make_pair(name, ComputeKernel{cl::Kernel{m_program, name}})).first->second;
}
