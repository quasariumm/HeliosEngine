#include "../backends/opengl46_glfw/graphics/gl46_texture_2d.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


namespace Helios
{
/*
 * Helper functions
 */

static std::pair<GLuint, GLuint> ConvertFormat( const TextureFormat format )
{
	switch (format)
	{
	case TextureFormat::RED8:
		return {GL_R8, 1};
	case TextureFormat::RED16:
		return {GL_R16, 2};
	case TextureFormat::RED32I:
		return {GL_R32I, 4};
	case TextureFormat::RED32UI:
		return {GL_R32UI, 4};
	case TextureFormat::RED16F:
		return {GL_R16F, 2};
	case TextureFormat::RED32F:
		return {GL_R32F, 4};
	case TextureFormat::RG8:
		return {GL_RG8, 1};
	case TextureFormat::RG16:
		return {GL_RG16, 2};
	case TextureFormat::RG16F:
		return {GL_RG16F, 2};
	case TextureFormat::RG32F:
		return {GL_RG32F, 4};
	case TextureFormat::RGB8:
		return {GL_RGB8, 1};
	case TextureFormat::RGB16F:
		return {GL_RGB16F, 2};
	case TextureFormat::RGB32F:
		return {GL_RGB32F, 4};
	case TextureFormat::RGBA8:
		return {GL_RGBA8, 1};
	case TextureFormat::RGBA16:
		return {GL_RGBA16, 2};
	case TextureFormat::RGBA16F:
		return {GL_RGBA16F, 2};
	case TextureFormat::RGBA32F:
		return {GL_RGBA32F, 4};
	default:
		return {GL_RGB8, 1};
	}
}


static GLenum ConvertBufferFormat( const TextureFormat format )
{
	switch (format)
	{
	case TextureFormat::RED8:
	case TextureFormat::RED16:
	case TextureFormat::RED32I:
	case TextureFormat::RED32UI:
		return GL_RED_INTEGER;
	case TextureFormat::RED16F:
	case TextureFormat::RED32F:
		return GL_RED;
	case TextureFormat::RG8:
	case TextureFormat::RG16:
		return GL_RG_INTEGER;
	case TextureFormat::RG16F:
	case TextureFormat::RG32F:
		return GL_RG;
	case TextureFormat::RGB8:
		return GL_RGB_INTEGER;
	case TextureFormat::RGB16F:
	case TextureFormat::RGB32F:
		return GL_RGB;
	case TextureFormat::RGBA8:
	case TextureFormat::RGBA16:
		return GL_RGBA_INTEGER;
	case TextureFormat::RGBA16F:
	case TextureFormat::RGBA32F:
		return GL_RGBA;
	default:
		return GL_RGB;
	}
}


static GLenum ConvertComponentType( const TextureFormat format )
{
	switch (format)
	{
	case TextureFormat::RED8:
	case TextureFormat::RG8:
	case TextureFormat::RGB8:
	case TextureFormat::RGBA8:
		return GL_UNSIGNED_BYTE;
	case TextureFormat::RG16:
	case TextureFormat::RED16:
	case TextureFormat::RGBA16:
		return GL_UNSIGNED_SHORT;
	case TextureFormat::RED32I:
		return GL_INT;
	case TextureFormat::RED32UI:
		return GL_UNSIGNED_INT;
	case TextureFormat::RED16F:
	case TextureFormat::RG16F:
	case TextureFormat::RGB16F:
	case TextureFormat::RGBA16F:
		return GL_HALF_FLOAT;
	case TextureFormat::RED32F:
	case TextureFormat::RG32F:
	case TextureFormat::RGB32F:
	case TextureFormat::RGBA32F:
		return GL_FLOAT;
	default:
		return GL_UNSIGNED_BYTE;
	}
}


/*
 * Class: GL64_Texture2D
 * Interface of Texture2D for OpenGL 4.6
 */

GL46_Texture2D::~GL46_Texture2D()
{
	if (m_initialized)
	{
		glDeleteTextures(1, &m_id);
		glDeleteBuffers(1, &m_pbo);
	}
}


void GL46_Texture2D::FillBlank(
		const uint32_t      width,
		const uint32_t      height,
		const uint32_t      channels,
		const TextureFormat format,
		const bool          isHDR
		)
{
	m_width    = static_cast<int32_t>(width);
	m_height   = static_cast<int32_t>(height);
	m_channels = static_cast<int32_t>(channels);
	m_isHdr    = isHDR;
	// Clean up if initialised
	if (m_initialized)
	{
		glDeleteTextures(1, &m_id);
		glDeleteBuffers(1, &m_pbo);
		m_pbo     = 0;
		m_data    = nullptr;
		m_dataHdr = nullptr;
	}

	// Generate GL texture
	m_glComponentType                 = ConvertComponentType(format);
	m_internalFormat                  = format;
	const auto [glFormat, formatSize] = ConvertFormat(format);
	m_glInternalFormat                = glFormat;
	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Create the buffer for the texture
	m_bufferFormat   = format;
	m_glBufferFormat = ConvertBufferFormat(format);
	glTexStorage2D(GL_TEXTURE_2D, 1, m_glInternalFormat, m_width, m_height);

	glGenBuffers(1, &m_pbo);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo);
	m_textureByteSize = m_width * m_height * m_channels * formatSize;
	glBufferStorage(GL_PIXEL_UNPACK_BUFFER, static_cast<GLsizeiptr>(m_textureByteSize), nullptr,
	                GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

	void* data = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, m_textureByteSize,
	                              GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

	if (isHDR)
		m_dataHdr = (float*)data;
	else
		m_data = (uint8_t*)data;

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	m_isStbBuffer = false;
	m_initialized = true;
}


void GL46_Texture2D::LoadFromFile(
		const std::string&  filename,
		const TextureFormat format,
		const bool          isHDR
		)
{
	// Clean up if initialised
	if (m_initialized)
	{
		glDeleteTextures(1, &m_id);
		glDeleteBuffers(1, &m_pbo);
		m_pbo = 0;
	}

	// Load the image
	m_isHdr    = isHDR;
	FILE* file = stbi__fopen(filename.c_str(), "rb");

	if (!file)
	{
		std::cout << ANSI_ERR << "Failed to open texture file " << filename << "!" << std::endl;
		throw std::runtime_error("Failed to open file");
	}

	if (isHDR)
		m_dataHdr = stbi_loadf_from_file(file, &m_width, &m_height, &m_channels, STBI_rgb);
	else
		m_data = stbi_load_from_file(file, &m_width, &m_height, &m_channels, STBI_rgb_alpha);

	fclose(file);

	m_glComponentType                 = ConvertComponentType(format);
	m_internalFormat                  = format;
	const auto [glFormat, formatSize] = ConvertFormat(format);
	m_glInternalFormat                = glFormat;
	m_bufferFormat                    = format;
	m_glBufferFormat                  = ConvertBufferFormat(format);

	glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	m_textureByteSize = m_width * m_height * m_channels * formatSize;
	glTexImage2D(GL_TEXTURE_2D, 0, m_glInternalFormat, m_width, m_height, 0, m_glBufferFormat, m_glComponentType,
	             (isHDR) ? (void*)m_dataHdr : m_data);
	glGenBuffers(1, &m_pbo);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo);
	glBufferStorage(GL_PIXEL_UNPACK_BUFFER, m_textureByteSize, m_dataHdr,
	                GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

	void* tmp = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, m_textureByteSize,
	                             GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

	if (isHDR)
	{
		memcpy(tmp, m_dataHdr, m_textureByteSize);
		stbi_image_free(m_dataHdr);
		m_dataHdr = (float*)tmp;
	}
	else
	{
		memcpy(tmp, m_data, m_textureByteSize);
		stbi_image_free(m_data);
		m_data = (uint8_t*)tmp;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	m_isStbBuffer = true;
	m_initialized = true;
}


void GL46_Texture2D::Use( const uint32_t slot, bool updateTexture ) const
{
	if (slot > 31)
		throw std::range_error("Slot is out of range (0-31)!");
	glActiveTexture(GL_TEXTURE0 + slot);
	if (updateTexture)
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo);
	glBindTexture(GL_TEXTURE_2D, m_id);
	if (updateTexture)
	{
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, m_glBufferFormat, m_glComponentType, nullptr);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	}
}


void GL46_Texture2D::UseCompute( const uint32_t slot, bool updateTexture ) const
{
	if (slot > 31)
		throw std::range_error("Slot is out of range (0-31)!");
	glActiveTexture(GL_TEXTURE0 + slot);
	if (updateTexture)
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo);
	glBindTexture(GL_TEXTURE_2D, m_id);
	if (updateTexture)
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, m_glBufferFormat, m_glComponentType, nullptr);
	glBindImageTexture(slot, m_id, 0, GL_FALSE, 0, GL_READ_WRITE, m_glInternalFormat);
	if (updateTexture)
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}


void GL46_Texture2D::UpdateData() const
{
	if (m_isHdr)
		glGetTextureSubImage(m_id, 0, 0, 0, 0, m_width, m_height, 1, m_glBufferFormat, m_glComponentType,
		                     m_textureByteSize, m_dataHdr);
	else
		glGetTextureSubImage(m_id, 0, 0, 0, 0, m_width, m_height, 1, m_glBufferFormat, m_glComponentType,
		                     m_textureByteSize, m_data);
}


uint32_t GL46_Texture2D::GetID() const
{
	return m_id;
}


int32_t GL46_Texture2D::GetWidth() const
{
	return m_width;
}


int32_t GL46_Texture2D::GetHeight() const
{
	return m_height;
}


uint32_t GL46_Texture2D::GetChannels() const
{
	return m_channels;
}


uint8_t* GL46_Texture2D::GetData() const
{
	return m_data;
}


float* GL46_Texture2D::GetDataHDR() const
{
	return m_dataHdr;
}
} // Engine
