#pragma once

#include <utility>


namespace Engine::Compute
{
template <typename T>
concept KernelArgs = std::is_base_of_v<cl::Memory, std::decay_t<T>> || std::is_arithmetic_v<std::decay_t<T>> || is_cl_vector_v<std::decay_t<T>>;

class Kernel
{

public:

	Kernel() = delete;


	/**
	 * @brief Sets one or a range of arguments to this kernel.
	 * @tparam Args The parameter pack. Use whatever the shader wants.
	 * @param startIdx The index where the first given argument should be linked to.
	 * @param args The parameter pack.
	 * @attention For any type in Engine::Compute, use the dereference operator (*) as the argument <br>
	 *		E.g.: kernel.SetArguments(*buffer, *texture);
	 */
	template<KernelArgs... Args>
	void SetArguments( int startIdx, Args&&... args )
	{
		(SetArgument(startIdx, std::forward<Args>(args)), ...);
	}


	/**
	 * @brief Runs the kernel once
	 */
	void Run() const;


	/**
	 * @brief Runs the kernel with the given number of threads
	 * @param groups The amount of groups. This is the amount of work groups, not the total executed threads.
	 * @param local The amount of threads per work group.
	 */
	void Run( const glm::uvec2& groups, const glm::uvec2& local = glm::uvec2{1} ) const;

	/**
	 * @brief Runs the kernel with the given number of threads
	 * @param groups The amount of groups. This is the amount of work groups, not the total executed threads.
	 * @param local The amount of threads per work group.
	 */
	void Run( const glm::uvec3& groups, const glm::uvec3& local = glm::uvec3{1} ) const;

	void PrintKernelArgs() const;


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

	template<typename T>
	void SetArgument( int& startIdx, T&& arg )
	{
		cl_int err;

		using DecayT = std::decay_t<T>;

		// For cl::Buffer, cl::Image, and other OpenCL C++ wrapper types
		if constexpr (std::is_base_of_v<cl::Memory, DecayT>)
		{
			err = m_kernel.setArg(startIdx++, std::forward<T>(arg));
		}
		// For all POD types (scalars, vectors, etc.)
		else
		{
			err = clSetKernelArg(m_kernel(), startIdx++, sizeof(DecayT), &arg);
		}

		if (err != CL_SUCCESS)
			Log::Error(std::format("Argument of type {} failed to set. Error: {}",
			                       typeid(arg).name(), CLErrorString(err)));
	}


	friend class Program;


	explicit Kernel( std::string name, cl::Kernel kernel )
		: m_name{std::move(name)}, m_kernel{std::move(kernel)}
	{
	};

	std::string m_name;
	cl::Kernel m_kernel;

};

} // namespace Engine::Graphics
