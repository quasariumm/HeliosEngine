#pragma once

#include <span>

#include "compute/program.hpp"


namespace Engine::Compute
{

template <typename T>
class Buffer
{

public:

	enum Access : uint16_t
	{
		DEVICE_READ_WRITE     = (1 << 0),
		DEVICE_WRITE_ONLY     = (1 << 1),
		DEVICE_READ_ONLY      = (1 << 2),
		DEVICE_USE_HOST_PTR   = (1 << 3),
		DEVICE_ALLOC_HOST_PTR = (1 << 4),
		DEVICE_COPY_HOST_PTR  = (1 << 5),
		HOST_WRITE_ONLY       = (1 << 7),
		HOST_READ_ONLY        = (1 << 8),
		HOST_NO_ACCESS        = (1 << 9)
	};


	using access_flag_t = uint16_t;

	/**
	 * @brief Constructs a new buffer using a pointer and element count
	 * @param data A pointer to the data
	 * @param numElements The number of elements to store
	 * @param access The access the host and device have to the buffer.
	 *		Default is using pointer from host memory.
	 *		For more efficient code, consider changing the access to include Access::DEVICE_COPY_HOST_PTR.
	 * @attention We prefer you use the constructor taking in an std::span<T> object.
	 */
	explicit Buffer( T* data, size_t numElements, access_flag_t access =
			                        Access::DEVICE_READ_WRITE | Access::DEVICE_USE_HOST_PTR
			);

	/**
	 * @brief Constructs a buffer using the more modern span object
	 * @param span The data as a span object.
	 */
	explicit Buffer( const std::span<T>& span, access_flag_t access =
			                        Access::DEVICE_READ_WRITE | Access::DEVICE_USE_HOST_PTR );


	void EnqueueRead();


	[[nodiscard]]
	cl::Buffer& GetBuffer() { return m_buffer; }


	[[nodiscard]]
	const cl::Buffer& GetBuffer() const { return m_buffer; }


	/**
	 * @brief Useful QoL operator to get the OpenCL object
	 * @return The OpenCL buffer object
	 */
	const cl::Buffer& operator*() const { return m_buffer; }

private:

	std::span<T> m_data;
	cl::Buffer   m_buffer;

};

/*
 * Implementation
 */

template <typename T>
Buffer<T>::Buffer( T* data, size_t numElements, access_flag_t access )
	: m_data{data, numElements}
{
	m_buffer = cl::Buffer{
			Program::m_context,
			access,
			sizeof(T) * numElements,
			data
	};
}


template <typename T>
Buffer<T>::Buffer( const std::span<T>& span, access_flag_t access )
	: m_data{span}
{
	m_buffer = cl::Buffer{
			Program::m_context,
			access,
			span.size_bytes(),
			span.data()
	};
}


template <typename T>
void Buffer<T>::EnqueueRead()
{
	Program::m_commandQueue.enqueueReadBuffer(m_buffer, CL_TRUE, 0, m_data.size_bytes(), m_data.data());
}

} // namespace Engine::Graphics
