#pragma once

#include "compute_kernel.hpp"


namespace Engine::Graphics
{

/**
 * Simple compute shader class. Takes the best GPU to dispatch work groups on
 */
class ComputeProgram
{

public:

	ComputeProgram() = default;

	static void Initialize();

	cl::Program& LoadFromFile( const std::filesystem::path& path );

	[[nodiscard]]
	ComputeKernel& GetKernel( const std::string& name );

	[[nodiscard]]
	cl::Program& GetProgram() { return m_program; }

	[[nodiscard]]
	const cl::Program& GetProgram() const { return m_program; }

	/**
	 * @brief Useful QoL operator to get the OpenCL object
	 * @return The OpenCL program object
	 */
	const cl::Program& operator*() const { return m_program; }

private:

	friend class ComputeKernel;
	template <typename T>
	friend class ComputeBuffer;

	static cl::Platform     m_platform;
	static cl::Device       m_device;
	static cl::Context      m_context;
	static cl::CommandQueue m_commandQueue;

	cl::Program m_program;

	std::map<std::string, ComputeKernel> m_loadedKernels;

};

} // namespace Engine::Graphics
