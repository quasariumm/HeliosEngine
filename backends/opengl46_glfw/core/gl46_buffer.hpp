#pragma once

namespace Engine
{

enum class BufferType : uint8_t
{
	ARRAY = 0,
	ELEMENT_ARRAY,
	COPY_READ,
	COPY_WRITE,
	PIXEL_UNPACK,
	PIXEL_PACK,
	QUERY,
	TEXTURE,
	TRANSFORM_FEEDBACK,
	UNIFORM,
	DRAW_INDIRECT,
	ATOMIC_COUNTER,
	DISPATCH_INDIRECT,
	SHADER_STORAGE,
	PARAMETER
};

enum class BufferAccess : uint8_t
{
	READ_ONLY = 0,
	WRITE_ONLY,
	READ_WRITE
};

class GL46_Buffer
{

public:

	GL46_Buffer() = default;

	void Init( BufferType type );

	void Bind();

	void BindBase( uint32_t base );

	void Clear() const;

	void Fill( size_t size, const void* data = nullptr ) const;

	void SubData( size_t offset, size_t size, const void* data = nullptr ) const;

	void* Map( BufferAccess access, size_t offset = -1, size_t length = -1 ) const;

	bool Unmap() const;

	[[nodiscard]]
	uint32_t ID() const { return m_bufferID; }

private:

	BufferType m_type;
	uint32_t m_bufferID = 0;

};

} // Engine
