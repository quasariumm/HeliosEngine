#pragma once

#include <utility>


namespace Engine::Graphics
{

class ComputeKernel
{

public:

	ComputeKernel() = delete;


	template <typename... Args>
	void SetArguments( int startIdx, Args&&... args )
	{
		(m_kernel.setArg(startIdx++, std::forward<Args>(args)), ...);
	}


	void Run( const glm::uvec2& groups, const glm::uvec2& local = glm::uvec2{1} ) const;

	void Run( const glm::uvec3& groups, const glm::uvec3& local = glm::uvec3{1} ) const;

	[[nodiscard]]
	cl::Kernel& GetKernel() { return m_kernel; }

	[[nodiscard]]
	const cl::Kernel& GetKernel() const { return m_kernel; }

	/**
	 * @brief Useful QoL operator to get the OpenCL object
	 * @return The OpenCL kernel object
	 */
	const cl::Kernel& operator*() const { return m_kernel; }

private:

	friend class ComputeProgram;


	explicit ComputeKernel( cl::Kernel kernel )
		: m_kernel{std::move(kernel)} {};

	cl::Kernel m_kernel;

};

} // namespace Engine::Graphics
