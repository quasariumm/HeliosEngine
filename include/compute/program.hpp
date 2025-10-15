#pragma once

#include "kernel.hpp"


namespace Engine
{
class GL46_Texture2D;
}


namespace Engine::Compute
{

/**
 * Simple compute shader class. Takes the best GPU to dispatch work groups on
 */
class Program
{

public:

	Program() = default;

	static void Initialize();

	cl::Program& LoadFromFile( const std::filesystem::path& path, const std::string& includeDir = "", const std::vector<std::string>& includeSources = {} );

	void Finish() const;

	[[nodiscard]]
	Kernel& GetKernel( const std::string& name );

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

	friend class Kernel;
	template <typename T, bool Owning>
	friend class Buffer;

	friend class ::Engine::GL46_Texture2D;
	friend class ::Engine::Renderer;

	static cl::Platform     m_platform;
	static cl::Device       m_device;
	static cl::Context      m_context;
	static cl::CommandQueue m_commandQueue;

	cl::Program m_program;

	std::map<std::string, Kernel> m_loadedKernels;

};

} // namespace Engine::Graphics
