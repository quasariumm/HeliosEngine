#pragma once

#include "compute/program.hpp"
#include "compute/access.hpp"

namespace Engine::Compute
{
static constexpr bool IMAGE_OWNING   = true;
static constexpr bool IMAGE_BORROWED = false;

template <typename T, bool Owning = IMAGE_OWNING>
	requires std::is_integral_v<T> || std::is_floating_point_v<T>
class Image2D
{
public:

	Image2D() = default;

	~Image2D();

	explicit Image2D(access_flag_t access);

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
	explicit Image2D(T*            data,
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
	explicit Image2D(const std::span<T>& span,
	                 uint32_t            width,
	                 uint32_t            height,
	                 uint8_t             numChannels,
	                 access_flag_t       access =
			                 Access_DEVICE_READ_WRITE | Access_DEVICE_USE_HOST_PTR);

	void ChangeData(T*       data,
	                size_t   numElements,
	                uint32_t width       = 0,
	                uint32_t height      = 0,
	                uint8_t  numChannels = 0);

	void ChangeData(const std::span<T>& span,
	                uint32_t            width       = 0,
	                uint32_t            height      = 0,
	                uint8_t             numChannels = 0);

	std::span<T>& GetData() { return m_data; }

	void UpdateDevicePointer();

	void EnqueueRead();


	[[nodiscard]]
	cl::Image2D& GetImage() { return m_image; }


	[[nodiscard]]
	const cl::Image2D& GetImage() const { return m_image; }


	/**
	 * @brief Useful QoL operator to get the OpenCL object
	 * @return The OpenCL buffer object
	 */
	const cl::Image2D& operator*() const { return m_image; }

private:

	void Init();

	static constexpr cl_int MapTypeToCLDefine();
	static cl_channel_order GetChannelOrder(uint32_t numChannels);

	access_flag_t           m_access;
	static constexpr cl_int TEXTURE_TYPE = MapTypeToCLDefine();
	cl_channel_order        m_channelType;
	uint32_t                m_width;
	uint32_t                m_height;
	std::span<T>            m_data;
	cl::Image2D             m_image;
};

template <bool Owning = IMAGE_OWNING>
using hdrImage2D_t = Image2D<float, Owning>;

template <typename T, bool Owning> requires std::is_integral_v<T> || std::is_floating_point_v<T>
Image2D<T, Owning>::~Image2D()
{
	const cl_int err = clReleaseMemObject(m_image.get());
	if (err != CL_SUCCESS)
		Log::Error(std::format("Failed to release buffer object: {}", CLErrorString(err)));

	if constexpr (Owning)
		free(m_data.data());
}

template <typename T, bool Owning>
	requires std::is_integral_v<T> || std::is_floating_point_v<T>
Image2D<T, Owning>::Image2D(const access_flag_t access) :
	m_access{access}, m_channelType{CL_RGBA}, m_width{0}, m_height{0}
{}


template <typename T, bool Owning> requires std::is_integral_v<T> || std::is_floating_point_v<T>
Image2D<T, Owning>::Image2D(T*                  data,
                            const size_t        numElements,
                            const uint32_t      width,
                            const uint32_t      height,
                            const uint8_t       numChannels,
                            const access_flag_t access) :
	m_access{access}, m_width{width}, m_height{height}, m_data{data, numElements}
{
	if constexpr (Owning)
	{
		m_data = std::span<T>{(T*)malloc(sizeof(T) * numElements), numElements};
		memcpy((void*)m_data.data(), (void*)data, numElements * sizeof(T));
	}
	m_channelType = GetChannelOrder(numChannels);
	Init();
}

template <typename T, bool Owning> requires std::is_integral_v<T> || std::is_floating_point_v<T>
Image2D<T, Owning>::Image2D(const std::span<T>& span,
                            const uint32_t      width,
                            const uint32_t      height,
                            const uint8_t       numChannels,
                            const access_flag_t access) :
	m_access{access}, m_width{width}, m_height{height}, m_data{span}
{
	if constexpr (Owning)
	{
		m_data = std::span<T>{(T*)malloc(sizeof(T) * span.size()), span.size()};
		memcpy((void*)m_data.data(), (void*)span.data(), span.size_bytes());
	}
	m_channelType = GetChannelOrder(numChannels);
	Init();
}

template <typename T, bool Owning> requires std::is_integral_v<T> || std::is_floating_point_v<T>
void Image2D<T, Owning>::ChangeData(T*             data,
                                    size_t         numElements,
                                    const uint32_t width,
                                    const uint32_t height,
                                    const uint8_t  numChannels)
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
	m_width = width;
	m_height = height;
	m_channelType = GetChannelOrder(numChannels);
	Init();
}

template <typename T, bool Owning> requires std::is_integral_v<T> || std::is_floating_point_v<T>
void Image2D<T, Owning>::ChangeData(const std::span<T>& span,
                                    const uint32_t      width,
                                    const uint32_t      height,
                                    const uint8_t       numChannels)
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
	m_width = width;
	m_height = height;
	m_channelType = GetChannelOrder(numChannels);
	Init();
}

template <typename T, bool Owning> requires std::is_integral_v<T> || std::is_floating_point_v<T>
void Image2D<T, Owning>::UpdateDevicePointer()
{
	Program::m_commandQueue.enqueueWriteImage(m_image, CL_BLOCKING, cl::array<size_t, 3>{0},
	                                          cl::array<size_t, 3>{m_width, m_height, 1}, 0, 0, m_data.data());
}

template <typename T, bool Owning> requires std::is_integral_v<T> || std::is_floating_point_v<T>
void Image2D<T, Owning>::EnqueueRead()
{
	Program::m_commandQueue.enqueueReadImage(m_image, CL_BLOCKING, cl::array<size_t, 3>{0},
	                                         cl::array<size_t, 3>{m_width, m_height, 1}, 0, 0, m_data.data());
}

template <typename T, bool Owning> requires std::is_integral_v<T> || std::is_floating_point_v<T>
void Image2D<T, Owning>::Init()
{
	const cl::ImageFormat image_format{
			m_channelType,
			TEXTURE_TYPE
	};
	cl_int err;
	m_image = cl::Image2D{
			Program::m_context,
			m_access,
			image_format,
			m_width,
			m_height,
			0,
			m_data.data(),
			&err
	};

	if (err != CL_SUCCESS)
		Log::Error(std::format("Failed to create image2d: {}", CLErrorString(err)));
}

template <typename T, bool Owning> requires std::is_integral_v<T> || std::is_floating_point_v<T>
cl_channel_order Image2D<T, Owning>::GetChannelOrder(const uint32_t numChannels)
{
	switch (numChannels)
	{
	case 1:
		return CL_R;
	case 2:
		return CL_RG;
	case 3:
		return CL_RGB;
	case 4:
		return CL_RGBA;
	default:
		Log::Error(std::format("Unsupported channel amount: {}", numChannels));
		return CL_RGB;
	}
}

template <typename T, bool Owning>
	requires std::is_integral_v<T> || std::is_floating_point_v<T>
constexpr cl_int Image2D<T, Owning>::MapTypeToCLDefine()
{
	if constexpr (std::is_same_v<T, float>)
		return CL_FLOAT;
	else if constexpr (std::is_same_v<T, cl_half>)
		return CL_HALF_FLOAT;
	else if constexpr (std::is_same_v<T, int8_t>)
		return CL_SIGNED_INT8;
	else if constexpr (std::is_same_v<T, int16_t>)
		return CL_SIGNED_INT16;
	else if constexpr (std::is_same_v<T, int32_t>)
		return CL_SIGNED_INT32;
	else if constexpr (std::is_same_v<T, uint8_t>)
		return CL_UNSIGNED_INT8;
	else if constexpr (std::is_same_v<T, uint16_t>)
		return CL_UNSIGNED_INT16;
	else if constexpr (std::is_same_v<T, uint32_t>)
		return CL_UNSIGNED_INT32;
	return CL_FLOAT;
}
}
