#pragma once
#include <cstdint>
#include <string>


namespace Engine
{

// TODO: Move to general texture file
enum class TextureFormat : uint8_t
{
	RED8 = 0u,	/* 1 channel  (8-bit int)			*/
	RED16,		/* 1 channel  (16-bit int)			*/
	RED16F,		/* 1 channel  (16-bit float)		*/
	RED32F,		/* 1 channel  (32-bit float) 		*/
	RG8,		/* 2 channels (8-bit int)			*/
	RG16,		/* 2 channels (16-bit int)			*/
	RG16F,		/* 2 channels (16-bit float)		*/
	RG32F,		/* 2 channels (32-bit float) 		*/
	RGB8,		/* 3 channels (8-bit int)			*/
	RGB16F,		/* 3 channels (16-bit 'half' float)	*/
	RGB32F,		/* 3 channels (32-bit float)		*/
	RGBA8,		/* 4 channels (8-bit int) 			*/
	RGBA16,		/* 4 channels (16-bit int) 			*/
	RGBA16F,	/* 4 channels (16-bit 'half' float)	*/
	RGBA32F		/* 4 channels (32-bit float)		*/
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
		uint32_t width, uint32_t height, uint32_t channels,
		TextureFormat format,
		bool isHDR
	);

	/**
	 * @brief Loads a texture from disk
	 * @param filename The relative path to the texture file from the working directory of the executable
	 * @param fileFormat The format of the image in the file
	 * @param internalFormat The format you want to save the texture as
	 * @param isHDR Whether the image should be treated as an HDR texture
	 * @warning Will delete old data if the image was initialised before
	 * @throws runtime_error The texture cannot be opened
	 */
	void LoadFromFile(
		const std::wstring& filename,
		TextureFormat fileFormat = TextureFormat::RGBA8, TextureFormat internalFormat = TextureFormat::RGBA8,
		bool isHDR = false
	);

	/**
	 * @brief Sets the texture to the active on the given slot
	 * @param slot The slot the texture needs to be in
	 * @throws range_error The slot is not in the bindable range
	 */
	void Use(uint32_t slot) const;

	/**
	 * @brief Sets the texture to the active on the given slot. Only used for compute shaders
	 * @param slot The slot the texture needs to be in
	 * @throws range_error The slot is not in the bindable range
	 */
	void UseCompute(uint32_t slot) const;

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

	int32_t m_width = 0;
	int32_t m_height = 0;
	int32_t m_channels = 0;

	uint32_t m_ID = 0;

	uint8_t* m_data = nullptr;
	float* m_dataHDR = nullptr;

	bool m_isHDR = false;
	bool m_isSTBBuffer= false;
	bool m_initialized = false;

};

} // Engine
