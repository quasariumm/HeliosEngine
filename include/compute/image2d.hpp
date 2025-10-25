#pragma once

#include "compute/program.hpp"
#include "compute/access.hpp"


namespace Engine::Compute
{
static constexpr bool IMAGE_OWNING   = true;
static constexpr bool IMAGE_BORROWED = false;


template <typename T, bool Owning = IMAGE_OWNING>
	requires std::is_same_v<std::decay_t<T>, uint8_t> || std::is_floating_point_v<T>
class Image2D
{
public:

	Image2D() = default;

	~Image2D();

	explicit Image2D( access_flag_t access );

	/**
	 * @brief Constructs a new buffer using a pointer and element count
	 * @param data A pointer to the data
	 * @param numElements The number of elements to store
	 * @param width Self-explanatory
	 * @param height Self-explanatory
	 * @param numChannels The number of channels used in the image
	 * @param access The access the host and device have to the buffer.
	 *		Default is using pointer from host memory.
	 *		For more efficient code, consider changing the access to include Access::DEVICE_COPY_HOST_PTR.
	 * @attention We prefer you use the constructor taking in an std::span<T> object.
	 */
	explicit Image2D( T*            data,
	                  size_t        numElements,
	                  uint32_t      width,
	                  uint32_t      height,
	                  uint8_t       numChannels,
	                  access_flag_t access =
			                  Access_DEVICE_READ_WRITE | Access_DEVICE_USE_HOST_PTR
			);

	/**
	 * @brief Constructs a buffer using the more modern span object
	 * @param span The data as a span object.
	 * @param width Self-explanatory
	 * @param height Self-explanatory
	 * @param numChannels The number of channels used in the image
	 * @param access The access the host and device have to the buffer.
	 *		Default is using pointer from host memory.
	 *		For more efficient code, consider changing the access to include Access::DEVICE_COPY_HOST_PTR
	 */
	explicit Image2D( const std::span<T>& span,
	                  uint32_t            width,
	                  uint32_t            height,
	                  uint8_t             numChannels,
	                  access_flag_t       access =
			                  Access_DEVICE_READ_WRITE | Access_DEVICE_USE_HOST_PTR );

	void ChangeData( T*       data,
	                 size_t   numElements,
	                 uint32_t width       = 0,
	                 uint32_t height      = 0,
	                 uint8_t  numChannels = 0 );

	void ChangeData( const std::span<T>& span,
	                 uint32_t            width       = 0,
	                 uint32_t            height      = 0,
	                 uint8_t             numChannels = 0 );

	std::span<T>& GetData() { return m_data; }

	void UpdateDevicePointer();

	void EnqueueRead();


	[[nodiscard]]
	cl::Buffer& GetImage() { return m_textureWrapper; }


	[[nodiscard]]
	const cl::Buffer& GetImage() const { return m_textureWrapper; }


	/**
	 * @brief Useful QoL operator to get the OpenCL object
	 * @return The OpenCL buffer object
	 */
	const cl::Buffer& operator*() const { return m_textureWrapper; }

private:

	void Init();


	struct GPUImage
	{
		cl_mem   data;
		uint32_t width;
		uint32_t height;
		uint8_t  channels;
	};


	access_flag_t m_access;
	std::span<T>  m_data;
	GPUImage      m_gpuImage;
	cl::Buffer    m_textureWrapper;
	cl::Buffer    m_image;
};


// TODO: Add support for other integer and float types
template <bool Owning = IMAGE_OWNING>
using hdrImage2D_t = Image2D<float, Owning>;
template <bool Owning = IMAGE_OWNING>
using byteImage2D_t = Image2D<uint8_t, Owning>;


template <typename T, bool Owning>
	requires std::is_same_v<std::decay_t<T>, uint8_t> || std::is_floating_point_v<T>
Image2D<T, Owning>::~Image2D()
{
	const cl_int err = clReleaseMemObject(m_image.get());
	if (err != CL_SUCCESS)
		Log::Error(std::format("Failed to release buffer object: {}", CLErrorString(err)));

	if constexpr (Owning)
		free(m_data.data());
}


template <typename T, bool Owning>
	requires std::is_same_v<std::decay_t<T>, uint8_t> || std::is_floating_point_v<T>
Image2D<T, Owning>::Image2D( const access_flag_t access ) :
	m_access{access}, m_gpuImage{nullptr, 0, 0, 0}
{}


template <typename T, bool Owning>
	requires std::is_same_v<std::decay_t<T>, uint8_t> || std::is_floating_point_v<T>
Image2D<T, Owning>::Image2D( T*                  data,
                             const size_t        numElements,
                             const uint32_t      width,
                             const uint32_t      height,
                             const uint8_t       numChannels,
                             const access_flag_t access ) :
	m_access{access}, m_data{data, numElements}, m_gpuImage{nullptr, width, height, numChannels}
{
	if constexpr (Owning)
	{
		m_data = std::span<T>{(T*)malloc(sizeof(T) * numElements), numElements};
		memcpy((void*)m_data.data(), (void*)data, numElements * sizeof(T));
	}
	Init();
}


template <typename T, bool Owning>
	requires std::is_same_v<std::decay_t<T>, uint8_t> || std::is_floating_point_v<T>
Image2D<T, Owning>::Image2D( const std::span<T>& span,
                             const uint32_t      width,
                             const uint32_t      height,
                             const uint8_t       numChannels,
                             const access_flag_t access ) :
	m_access{access}, m_data{span}, m_gpuImage{nullptr, width, height, numChannels}
{
	if constexpr (Owning)
	{
		m_data = std::span<T>{(T*)malloc(sizeof(T) * span.size()), span.size()};
		memcpy((void*)m_data.data(), (void*)span.data(), span.size_bytes());
	}
	Init();
}


template <typename T, bool Owning>
	requires std::is_same_v<std::decay_t<T>, uint8_t> || std::is_floating_point_v<T>
void Image2D<T, Owning>::ChangeData( T*             data,
                                     size_t         numElements,
                                     const uint32_t width,
                                     const uint32_t height,
                                     const uint8_t  numChannels )
{
	if constexpr (Owning)
	{
		free(m_data.data());
		m_data = std::span<T>{(T*)malloc(sizeof(T) * numElements), numElements};
		memcpy((void*)m_data.data(), (void*)data, numElements * sizeof(T));
	}
	else
	{
		m_data = std::span<T>{data, numElements};
	}
	m_gpuImage.width    = width;
	m_gpuImage.height   = height;
	m_gpuImage.channels = numChannels;
	Init();
}


template <typename T, bool Owning>
	requires std::is_same_v<std::decay_t<T>, uint8_t> || std::is_floating_point_v<T>
void Image2D<T, Owning>::ChangeData( const std::span<T>& span,
                                     const uint32_t      width,
                                     const uint32_t      height,
                                     const uint8_t       numChannels )
{
	if constexpr (Owning)
	{
		free(m_data.data());
		m_data = std::span<T>{(T*)malloc(sizeof(T) * span.size()), span.size()};
		memcpy((void*)m_data.data(), (void*)span.data(), span.size_bytes());
	}
	else
	{
		m_data = span;
	}
	m_gpuImage.width    = width;
	m_gpuImage.height   = height;
	m_gpuImage.channels = numChannels;
	Init();
}


template <typename T, bool Owning>
	requires std::is_same_v<std::decay_t<T>, uint8_t> || std::is_floating_point_v<T>
void Image2D<T, Owning>::UpdateDevicePointer()
{
	Program::m_commandQueue.enqueueReadBuffer(m_image, CL_BLOCKING, 0,
	                                          m_gpuImage.width * m_gpuImage.height * m_gpuImage.channels * sizeof(T),
	                                          m_data.data());
}


template <typename T, bool Owning>
	requires std::is_same_v<std::decay_t<T>, uint8_t> || std::is_floating_point_v<T>
void Image2D<T, Owning>::EnqueueRead()
{
	Program::m_commandQueue.enqueueReadBuffer(m_image, CL_BLOCKING, 0,
	                                          m_gpuImage.width * m_gpuImage.height * m_gpuImage.channels * sizeof(T),
	                                          m_data.data());
}


template <typename T, bool Owning>
	requires std::is_same_v<std::decay_t<T>, uint8_t> || std::is_floating_point_v<T>
void Image2D<T, Owning>::Init()
{
	cl_int err;
	m_image = cl::Buffer{
			Program::m_context,
			m_access,
			m_gpuImage.width * m_gpuImage.height * m_gpuImage.channels,
			m_data.data(),
			&err
	};
	if (err != CL_SUCCESS)
		Log::Error(std::format("Failed to create image buffer: {}", CLErrorString(err)));
	m_gpuImage.data  = m_image.get();
	m_textureWrapper = cl::Buffer{
			Program::m_context,
			m_access,
			sizeof(GPUImage),
			&m_gpuImage,
			&err
	};
	if (err != CL_SUCCESS)
		Log::Error(std::format("Failed to create image wrapper: {}", CLErrorString(err)));
}
}
