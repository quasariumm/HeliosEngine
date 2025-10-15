#include "compute/kernel.hpp"

#include "compute/program.hpp"
#include "debugging/logger.hpp"

using namespace Engine::Compute;


void Kernel::Run( const glm::uvec2& groups, const glm::uvec2& local ) const
{
	auto err = Program::m_commandQueue.enqueueNDRangeKernel(m_kernel, cl::NullRange,
	                                                        cl::NDRange{static_cast<uint64_t>(local.x * groups.x),
	                                                                    static_cast<uint64_t>(local.y * groups.y)},
	                                                        cl::NDRange{static_cast<uint64_t>(local.x),
	                                                                    static_cast<uint64_t>(local.y)});

	if (err != CL_SUCCESS)
		Log::Error(std::format("Compute kernel dispatch failed. Error: {}", CLErrorString(err)));
}


void Kernel::Run( const glm::uvec3& groups, const glm::uvec3& local ) const
{
	auto err = Program::m_commandQueue.enqueueNDRangeKernel(m_kernel, cl::NullRange,
	                                                        cl::NDRange{static_cast<uint64_t>(local.x * groups.x),
	                                                                    static_cast<uint64_t>(local.y * groups.y),
	                                                                    static_cast<uint64_t>(local.z * groups.z)},
	                                                        cl::NDRange{static_cast<uint64_t>(local.x),
	                                                                    static_cast<uint64_t>(local.y),
	                                                                    static_cast<uint64_t>(local.z)});

	if (err != CL_SUCCESS)
		Log::Error(std::format("Compute kernel dispatch failed. Error: {}", CLErrorString(err)));
}


void Kernel::PrintKernelArgs() const
{
	cl_uint num_args;

	// Get the number of arguments
	const cl_int err = clGetKernelInfo(m_kernel.get(), CL_KERNEL_NUM_ARGS,
	                                   sizeof(cl_uint), &num_args, nullptr);
	if (err != CL_SUCCESS)
	{
		Log::Error("Failed to get number of kernel arguments\n");
		return;
	}

	Log::Info(std::format("Kernel has {} arguments:", num_args));

	// Iterate through each argument
	for (cl_uint i = 0; i < num_args; i++)
	{
		// Get argument name
		size_t name_size;
		clGetKernelArgInfo(m_kernel.get(), i, CL_KERNEL_ARG_NAME, 0, nullptr, &name_size);
		const auto arg_name = static_cast<char*>(malloc(name_size));
		clGetKernelArgInfo(m_kernel.get(), i, CL_KERNEL_ARG_NAME, name_size, arg_name, nullptr);

		// Get argument type
		size_t type_size;
		clGetKernelArgInfo(m_kernel.get(), i, CL_KERNEL_ARG_TYPE_NAME, 0, nullptr, &type_size);
		const auto arg_type = static_cast<char*>(malloc(type_size));
		clGetKernelArgInfo(m_kernel.get(), i, CL_KERNEL_ARG_TYPE_NAME, type_size, arg_type, nullptr);

		// Get address qualifier
		cl_kernel_arg_address_qualifier addr_qual;
		clGetKernelArgInfo(m_kernel.get(), i, CL_KERNEL_ARG_ADDRESS_QUALIFIER,
		                   sizeof(addr_qual), &addr_qual, nullptr);

		auto addr_str = "private";
		switch (addr_qual)
		{
		case CL_KERNEL_ARG_ADDRESS_GLOBAL:
			addr_str = "global";
			break;
		case CL_KERNEL_ARG_ADDRESS_LOCAL:
			addr_str = "local";
			break;
		case CL_KERNEL_ARG_ADDRESS_CONSTANT:
			addr_str = "constant";
			break;
		default:
			break;
		}

		Log::Info(std::format("  Arg {}: {} {} {}", i, addr_str, arg_type, arg_name));

		free(arg_name);
		free(arg_type);
	}
}
