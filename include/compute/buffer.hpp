#pragma once

#include <span>

#include "compute/program.hpp"


namespace Engine::Compute
{

enum BufferAccess : uint16_t
{
	BufferAccess_DEVICE_READ_WRITE     = (1 << 0),
	BufferAccess_DEVICE_WRITE_ONLY     = (1 << 1),
	BufferAccess_DEVICE_READ_ONLY      = (1 << 2),
	BufferAccess_DEVICE_USE_HOST_PTR   = (1 << 3),
	BufferAccess_DEVICE_ALLOC_HOST_PTR = (1 << 4),
	BufferAccess_DEVICE_COPY_HOST_PTR  = (1 << 5),
	BufferAccess_HOST_WRITE_ONLY       = (1 << 7),
	BufferAccess_HOST_READ_ONLY        = (1 << 8),
	BufferAccess_HOST_NO_ACCESS        = (1 << 9),

	// Common use-cases
	BufferAccess_COPIED_READ_ONLY	   = BufferAccess_DEVICE_READ_ONLY | BufferAccess_DEVICE_COPY_HOST_PTR,
};

using access_flag_t = uint16_t;

static constexpr bool BUFFER_OWNING = true;
static constexpr bool BUFFER_BORROWED = false;

template <typename T, bool Owning = BUFFER_OWNING>
class Buffer
{

public:

	Buffer() = default;

	~Buffer();

	explicit Buffer( const access_flag_t access )
		: m_access{access} {}

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
			                 BufferAccess_DEVICE_READ_WRITE | BufferAccess_DEVICE_USE_HOST_PTR
			);

	/**
	 * @brief Constructs a buffer using the more modern span object
	 * @param span The data as a span object.
	 */
	explicit Buffer( const std::span<T>& span, access_flag_t access =
			                 BufferAccess_DEVICE_READ_WRITE | BufferAccess_DEVICE_USE_HOST_PTR );

	void ChangeData( T* data, size_t numElements );

	void ChangeData( const std::span<T>& span );

	std::span<T>& GetData() { return m_data; }

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

	access_flag_t m_access;
	std::span<T>  m_data;
	cl::Buffer    m_buffer;

};


/*
 * Implementation
 */

template <typename T, bool Owning>
Buffer<T, Owning>::~Buffer()
{
	if constexpr (Owning)
		free(m_data.data());
}


template <typename T, bool Owning>
Buffer<T, Owning>::Buffer( T* data, size_t numElements, access_flag_t access )
	: m_access{access}, m_data{data, numElements}
{
	if constexpr (Owning)
	{
		m_data = std::span<T>{(T*)malloc(sizeof(T) * numElements), numElements};
		memcpy((void*)m_data.data(), (void*)data, numElements * sizeof(T));
	}
	m_buffer = cl::Buffer{
			Program::m_context,
			access,
			sizeof(T) * numElements,
			data
	};
}


template <typename T, bool Owning>
Buffer<T, Owning>::Buffer( const std::span<T>& span, access_flag_t access )
	: m_access{access}, m_data{span}
{
	if constexpr (Owning)
	{
		m_data = std::span<T>{(T*)malloc(sizeof(T) * span.size()), span.size()};
		memcpy((void*)m_data.data(), (void*)span.data(), span.size_bytes());
	}
	m_buffer = cl::Buffer{
			Program::m_context,
			access,
			span.size_bytes(),
			span.data()
	};
}


template <typename T, bool Owning>
void Buffer<T, Owning>::ChangeData( T* data, size_t numElements )
{
	if constexpr (Owning)
	{
		delete m_data.data();
		m_data = std::span<T>{(T*)malloc(sizeof(T) * numElements), numElements};
		memcpy((void*)m_data.data(), (void*)data, numElements * sizeof(T));
	}
	else
	{
		m_data = std::span<T>{data, numElements};
	}
	m_buffer = cl::Buffer{
			Program::m_context,
			m_access,
			sizeof(T) * numElements,
			data
	};
}


template <typename T, bool Owning>
void Buffer<T, Owning>::ChangeData( const std::span<T>& span )
{
	if constexpr (Owning)
	{
		delete m_data.data();
		m_data = std::span<T>{(T*)malloc(sizeof(T) * span.size()), span.size()};
		memcpy((void*)m_data.data(), (void*)span.data(), span.size_bytes());
	}
	else
	{
		m_data = span;
	}
	m_buffer = cl::Buffer{
		Program::m_context,
		m_access,
		span.size_bytes(),
		span.data()
	};
}


template <typename T, bool Owning>
void Buffer<T, Owning>::EnqueueRead()
{
	auto err = Program::m_commandQueue.enqueueReadBuffer(m_buffer, CL_TRUE, 0, m_data.size_bytes(), m_data.data());

	if (err != CL_SUCCESS)
		Log::Error(std::format("Failed to enqueue a read. Error: {}", CLErrorString(err)));
}

} // namespace Engine::Graphics
