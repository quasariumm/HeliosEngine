#include "compute_kernel.hpp"

#include "compute_program.hpp"
#include "Debugger/Debugger.h"

using namespace Engine::Graphics;


void ComputeKernel::Run( const glm::uvec2& groups, const glm::uvec2& local ) const
{
	auto err = ComputeProgram::m_commandQueue.enqueueNDRangeKernel(m_kernel, cl::NullRange,
	                                                               cl::NDRange{static_cast<uint64_t>(groups.x),
	                                                                           static_cast<uint64_t>(groups.y)},
	                                                               cl::NDRange{static_cast<uint64_t>(local.x),
	                                                                           static_cast<uint64_t>(local.y)});

	if (err != CL_SUCCESS)
		DebugLog(LogSeverity::SEVERE, std::format(L"Compute kernel dispatch failed. Error: {}", CLErrorString(err)));
}


void ComputeKernel::Run( const glm::uvec3& groups, const glm::uvec3& local ) const
{
	auto err = ComputeProgram::m_commandQueue.enqueueNDRangeKernel(m_kernel, cl::NullRange,
	                                                               cl::NDRange{static_cast<uint64_t>(groups.x),
	                                                                           static_cast<uint64_t>(groups.y),
	                                                                           static_cast<uint64_t>(groups.z)},
	                                                               cl::NDRange{static_cast<uint64_t>(local.x),
	                                                                           static_cast<uint64_t>(local.y),
	                                                                           static_cast<uint64_t>(local.z)});

	if (err != CL_SUCCESS)
		DebugLog(LogSeverity::SEVERE, std::format(L"Compute kernel dispatch failed. Error: {}", CLErrorString(err)));
}
