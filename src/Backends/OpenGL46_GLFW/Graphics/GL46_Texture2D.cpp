#include "GL46_Texture2D.h"

#include <glad/glad.h>
#include <iostream>
#include <ostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Core/Common.h"


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
	glDeleteTextures(1, &m_ID);
	if (m_isSTBBuffer)
	{
		if (m_data)
			stbi_image_free(m_data);
		if (m_dataHDR)
			stbi_image_free(m_dataHDR);
	}
	else
	{
		delete[] m_data;
		delete[] m_dataHDR;
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
	// Clean up if initialised
	if (m_initialized)
	{
		glDeleteTextures(1, &m_ID);
		if (m_isSTBBuffer)
		{
			if (m_data)
				stbi_image_free(m_data);
			if (m_dataHDR)
				stbi_image_free(m_dataHDR);
		}
		else
		{
			delete[] m_data;
			delete[] m_dataHDR;
		}
		m_data = nullptr;
		m_dataHDR = nullptr;
	}

	// Generate GL texture
	const GLenum intFormat = ConvertFormat(format);
	glGenTextures(1, &m_ID);
	glBindTexture(GL_TEXTURE_2D, m_ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Create the buffer for the texture
	GLenum bufferFormat;
	switch (m_channels)
	{
	case 1:
		bufferFormat = GL_RED;
		break;
	case 2:
		bufferFormat = GL_RG;
		break;
	case 3:
		bufferFormat = GL_RGB;
		break;
	case 4:
		bufferFormat = GL_RGBA;
		break;
	default:
		bufferFormat = GL_RGB;
		break;
	}
	if (isHDR)
	{
		m_dataHDR = new float[m_width * m_height * m_channels];
		glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(intFormat), m_width, m_height, 0, bufferFormat, GL_FLOAT, m_dataHDR);
	}
	else
	{
		m_data = new uint8_t[m_width * m_height * m_channels];
		glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(intFormat), m_width, m_height, 0, bufferFormat, GL_UNSIGNED_BYTE, m_data);
	}

	glBindTexture(GL_TEXTURE_2D, 0);

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
		if (m_isSTBBuffer)
		{
			if (m_data)
				stbi_image_free(m_data);
			if (m_dataHDR)
				stbi_image_free(m_dataHDR);
		}
		else
		{
			delete[] m_data;
			delete[] m_dataHDR;
		}
		m_data = nullptr;
		m_dataHDR = nullptr;
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

	const GLenum format = ConvertFormat(fileFormat);
	const GLint intFormat = static_cast<GLint>(ConvertFormat(internalFormat));

	glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);
	glBindTexture(GL_TEXTURE_2D, m_ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (isHDR)
		glTexImage2D(GL_TEXTURE_2D, 0, intFormat, m_width, m_height, 0, format, GL_FLOAT, m_dataHDR);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, intFormat, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, m_data);
	glBindTexture(GL_TEXTURE_2D, 0);

	m_isSTBBuffer = true;
}


void GL46_Texture2D::Use(const uint32_t slot) const
{
	if (slot > 31)
		throw std::range_error("Slot is out of range (0-31)!");
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_ID);
}


void GL46_Texture2D::UseCompute( const uint32_t slot ) const
{
	if (slot > 31)
		throw std::range_error("Slot is out of range (0-31)!");
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_ID);
	// TODO: Update to use dynamic format
	glBindImageTexture(slot, m_ID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
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