#include "GL46_Texture2D.h"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


namespace Engine
{

/*
 * Helper functions
 */

static GLuint ConvertFormat(const TextureFormat format)
{
	switch (format)
	{
	case TextureFormat::RED8:
		return GL_R8;
	case TextureFormat::RED16:
		return GL_R16;
	case TextureFormat::RED16F:
		return GL_R16F;
	case TextureFormat::RED32F:
		return GL_R32F;
	case TextureFormat::RG8:
		return GL_RG8;
	case TextureFormat::RG16:
		return GL_RG16;
	case TextureFormat::RG16F:
		return GL_RG16F;
	case TextureFormat::RG32F:
		return GL_RG32F;
	case TextureFormat::RGB8:
		return GL_RGB8;
	case TextureFormat::RGB16F:
		return GL_RGB16F;
	case TextureFormat::RGB32F:
		return GL_RGB32F;
	case TextureFormat::RGBA8:
		return GL_RGBA8;
	case TextureFormat::RGBA16:
		return GL_RGBA16;
	case TextureFormat::RGBA16F:
		return GL_RGBA16F;
	case TextureFormat::RGBA32F:
		return GL_RGBA32F;
	default:
		return GL_RGB8;
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
		glDeleteTextures(1, &m_ID);
		glDeleteBuffers(1, &m_pbo);
	}
}


void GL46_Texture2D::FillBlank(
	const uint32_t width, const uint32_t height, const uint32_t channels,
	const TextureFormat format,
	const bool isHDR
	)
{
	m_width = static_cast<int32_t>(width);
	m_height = static_cast<int32_t>(height);
	m_channels = static_cast<int32_t>(channels);
	m_isHDR = isHDR;
	// Clean up if initialised
	if (m_initialized)
	{
		glDeleteTextures(1, &m_ID);
		glDeleteBuffers(1, &m_pbo);
		m_pbo = 0;
		m_data = nullptr;
		m_dataHDR = nullptr;
	}

	// Generate GL texture
	m_internalFormat = format;
	m_glInternalFormat = ConvertFormat(format);
	glGenTextures(1, &m_ID);
	glBindTexture(GL_TEXTURE_2D, m_ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Create the buffer for the texture
	switch (m_channels)
	{
	case 1:
		m_bufferFormat = (isHDR) ? TextureFormat::RED32F : TextureFormat::RED8;
		m_glBufferFormat = GL_RED;
		break;
	case 2:
		m_bufferFormat = (isHDR) ? TextureFormat::RG32F : TextureFormat::RG8;
		m_glBufferFormat = GL_RG;
		break;
	case 3:
		m_bufferFormat = (isHDR) ? TextureFormat::RGB32F : TextureFormat::RGB8;
		m_glBufferFormat = GL_RGB;
		break;
	case 4:
		m_bufferFormat = (isHDR) ? TextureFormat::RGBA32F : TextureFormat::RGBA8;
		m_glBufferFormat = GL_RGBA;
		break;
	default:
		m_bufferFormat = (isHDR) ? TextureFormat::RGB32F : TextureFormat::RGB8;
		m_glBufferFormat = GL_RGB;
		break;
	}
	glTexStorage2D(GL_TEXTURE_2D, 1, ConvertFormat(m_internalFormat), m_width, m_height);
	if (isHDR)
	{
		glGenBuffers(1, &m_pbo);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo);
		m_textureByteSize = m_width * m_height * m_channels * sizeof(float);
		glBufferStorage(GL_PIXEL_UNPACK_BUFFER, m_textureByteSize, nullptr, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
		m_dataHDR = (float*)glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, m_textureByteSize, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
	}
	else
	{
		glGenBuffers(1, &m_pbo);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo);
		m_textureByteSize = m_width * m_height * m_channels * sizeof(uint8_t);
		glBufferStorage(GL_PIXEL_UNPACK_BUFFER, m_textureByteSize, nullptr, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
		m_data = (uint8_t*)glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, m_textureByteSize, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	m_isSTBBuffer = false;
}


void GL46_Texture2D::LoadFromFile(
	const std::wstring& filename,
	const TextureFormat fileFormat, const TextureFormat internalFormat,
	const bool isHDR
	)
{
	// Clean up if initialised
	if (m_initialized)
	{
		glDeleteTextures(1, &m_ID);
		glDeleteBuffers(1, &m_pbo);
		m_pbo = 0;
	}

	// Load the image
	m_isHDR = isHDR;
	FILE* file = _wfopen(filename.c_str(), L"rb");

	if (!file)
	{
		std::wcout << ANSI_ERR << "Failed to open texture file " << filename << "!" << std::endl;
		throw std::runtime_error("Failed to open file");
	}

	if (isHDR)
		m_dataHDR = stbi_loadf_from_file(file, &m_width, &m_height, &m_channels, STBI_rgb);
	else
		m_data = stbi_load_from_file(file, &m_width, &m_height, &m_channels, STBI_rgb_alpha);

	fclose(file);

	m_internalFormat = internalFormat;
	m_glInternalFormat = ConvertFormat(internalFormat);
	m_bufferFormat = fileFormat;
	m_glBufferFormat = ConvertFormat(fileFormat);

	glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);
	glBindTexture(GL_TEXTURE_2D, m_ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (isHDR)
	{
		m_textureByteSize = m_width * m_height * m_channels * sizeof(float);
		glTexImage2D(GL_TEXTURE_2D, 0, m_glInternalFormat, m_width, m_height, 0, m_glBufferFormat, GL_FLOAT, m_dataHDR);
		glGenBuffers(1, &m_pbo);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo);
		glBufferStorage(GL_PIXEL_UNPACK_BUFFER, sizeof(m_dataHDR), m_dataHDR, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
		float* tmp = (float*)glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, sizeof(m_dataHDR), GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
		memcpy(tmp, m_dataHDR, sizeof(m_dataHDR));
		stbi_image_free(m_dataHDR);
		m_dataHDR = tmp;
	}
	else
	{
		m_textureByteSize = m_width * m_height * m_channels * sizeof(uint8_t);
		glTexImage2D(GL_TEXTURE_2D, 0, m_glInternalFormat, m_width, m_height, 0, m_glBufferFormat, GL_UNSIGNED_BYTE, m_data);
		glGenBuffers(1, &m_pbo);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo);
		glBufferStorage(GL_PIXEL_UNPACK_BUFFER, sizeof(m_data), m_data, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
		uint8_t* tmp = (uint8_t*)glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, sizeof(m_data), GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
		memcpy(tmp, m_data, sizeof(m_data));
		stbi_image_free(m_data);
		m_data = tmp;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	m_isSTBBuffer = true;
}


void GL46_Texture2D::Use(const uint32_t slot) const
{
	if (slot > 31)
		throw std::range_error("Slot is out of range (0-31)!");
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo);
	glBindTexture(GL_TEXTURE_2D, m_ID);
	const GLenum type = (m_isHDR) ? GL_FLOAT : GL_UNSIGNED_BYTE;
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, m_glBufferFormat, type, nullptr);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}


void GL46_Texture2D::UseCompute( const uint32_t slot ) const
{
	if (slot > 31)
		throw std::range_error("Slot is out of range (0-31)!");
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo);
	glBindTexture(GL_TEXTURE_2D, m_ID);
	const GLenum type = (m_isHDR) ? GL_FLOAT : GL_UNSIGNED_BYTE;
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, m_glBufferFormat, type, nullptr);
	glBindImageTexture(slot, m_ID, 0, GL_FALSE, 0, GL_READ_WRITE, m_glInternalFormat);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}


void GL46_Texture2D::UpdateData() const
{
	if (m_isHDR)
		glGetTextureSubImage(m_ID, 0, 0, 0, 0, m_width, m_height, 1, m_glBufferFormat, GL_FLOAT, m_textureByteSize, m_dataHDR);
	else
		glGetTextureSubImage(m_ID, 0, 0, 0, 0, m_width, m_height, 1, m_glBufferFormat, GL_UNSIGNED_BYTE, m_textureByteSize, m_data);
}


uint32_t GL46_Texture2D::GetID() const
{
	return m_ID;
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
	return m_dataHDR;
}

} // Engine