// ReSharper disable CppInconsistentNaming
#pragma once

namespace Helios
{
// TODO: Move to general texture file
enum class TextureFormat : uint8_t
{
	RED8 = 0u,
	/* 1 channel  (8-bit int)			*/
	RED16,
	/* 1 channel  (16-bit int)			*/
	RED32I,
	/* 1 channel  (32-bit int)			*/
	RED32UI,
	/* 1 channel  (32-bit unsigned int) */
	RED16F,
	/* 1 channel  (16-bit float)		*/
	RED32F,
	/* 1 channel  (32-bit float) 		*/
	RG8,
	/* 2 channels (8-bit int)			*/
	RG16,
	/* 2 channels (16-bit int)			*/
	RG16F,
	/* 2 channels (16-bit float)		*/
	RG32F,
	/* 2 channels (32-bit float) 		*/
	RGB8,
	/* 3 channels (8-bit int)			*/
	RGB16F,
	/* 3 channels (16-bit 'half' float)	*/
	RGB32F,
	/* 3 channels (32-bit float)		*/
	RGBA8,
	/* 4 channels (8-bit int) 			*/
	RGBA16,
	/* 4 channels (16-bit int) 			*/
	RGBA16F,
	/* 4 channels (16-bit 'half' float)	*/
	RGBA32F /* 4 channels (32-bit float)		*/
};


class GL46_Texture2D
{
public:

	GL46_Texture2D() = default;

	~GL46_Texture2D();

	/**
	 * @brief Fills in the image with blank data
	 * @param width The width in pixels
	 * @param height The height in pixels
	 * @param channels The amount of channels per pixel
	 * @param format The format of the stored texture
	 * @param isHDR Whether the image should be HDR
	 * @warning Will delete old data if the image was initialised before
	 */
	void FillBlank(
			uint32_t      width,
			uint32_t      height,
			uint32_t      channels,
			TextureFormat format,
			bool          isHDR
			);

	/**
	 * @brief Loads a texture from disk
	 * @param filename The relative path to the texture file from the working directory of the executable
	 * @param format The format of the image in the file
	 * @param isHDR Whether the image should be treated as an HDR texture
	 * @warning Will delete old data if the image was initialised before
	 * @throws runtime_error The texture cannot be opened
	 */
	void LoadFromFile(
			const std::string& filename,
			TextureFormat      format = TextureFormat::RGBA8,
			bool               isHDR  = false
			);

	/**
	 * @brief Sets the texture to the active on the given slot
	 * @param slot The slot the texture needs to be in
	 * @param updateTexture Whether the texture should be filled with the data in this object
	 * @throws range_error The slot is not in the bindable range
	 */
	void Use( uint32_t slot, bool updateTexture = false ) const;

	/**
	 * @brief Sets the texture to the active on the given slot. Only used for compute shaders
	 * @param slot The slot the texture needs to be in
	 * @param updateTexture Whether the texture should be filled with the data in this object
	 * @throws range_error The slot is not in the bindable range
	 */
	void UseCompute( uint32_t slot, bool updateTexture = false ) const;

	/**
	 * @brief Updates the pixel unwrap buffer to the data in the texture
	 */
	void UpdateData() const;

	template <typename T>
	T* GetPixel( glm::uvec2 position ) const;

	/**
	 * @brief Gets the ID of the texture
	 * @return The ID of the texture
	 */
	[[nodiscard]]
	uint32_t GetID() const;

	/**
	 * @brief Gets the width of the texture
	 * @return The width of the texture
	 */
	[[nodiscard]]
	int32_t GetWidth() const;

	/**
	 * @brief Gets the height of the texture
	 * @return The height of the texture
	 */
	[[nodiscard]]
	int32_t GetHeight() const;

	/**
	 * @brief Gets the channel count of the texture
	 * @return The channel count of the texture
	 */
	[[nodiscard]]
	uint32_t GetChannels() const;

	/**
	 * @brief Gives you the data pointer of the texture
	 * @return A pointer to the first pixel in the texture data
	 * @warning Will return nullptr when the texture is HDR. In that case, call GetDataHDR() instead.
	 */
	[[nodiscard]]
	uint8_t* GetData() const;

	/**
	 * @brief Gives you the data pointer of the texture
	 * @return A pointer to the first pixel in the texture data
	 * @warning Will return nullptr when the texture is not HDR. In that case, call GetData() instead.
	 */
	[[nodiscard]]
	float* GetDataHDR() const;

private:

	TextureFormat m_internalFormat   = TextureFormat::RGBA8;
	uint32_t      m_glInternalFormat = 0;
	TextureFormat m_bufferFormat     = TextureFormat::RGBA8;
	uint32_t      m_glBufferFormat   = 0;

	uint32_t m_glComponentType = 0;

	int32_t m_width           = 0;
	int32_t m_height          = 0;
	int32_t m_channels        = 0;
	size_t  m_textureByteSize = 0ull;

	uint32_t m_id  = 0;
	uint32_t m_pbo = 0;

	uint8_t* m_data    = nullptr;
	float*   m_dataHdr = nullptr;

	bool m_isHdr       = false;
	bool m_isStbBuffer = false;
	bool m_initialized = false;
};


template <typename T>
T* GL46_Texture2D::GetPixel( const glm::uvec2 position ) const
{
	if (position.x >= m_width || position.y >= m_height)
		return nullptr;

	UpdateData();
	if (m_isHdr)
		return &(((T*)m_dataHdr)[(position.y * m_width + position.x) * m_channels]);

	return &(((T*)m_data)[(position.y * m_width + position.x) * m_channels]);
}
} // Engine
