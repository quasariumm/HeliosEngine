#include "gl46_buffer.hpp"

// TODO(Quillan): DEBUGGER
//#include "debugging/logger.hpp"


namespace Engine
{

constexpr static uint32_t GLBufferType[] = {
	GL_ARRAY_BUFFER,
	GL_ELEMENT_ARRAY_BUFFER,
	GL_COPY_READ_BUFFER,
	GL_COPY_WRITE_BUFFER,
	GL_PIXEL_UNPACK_BUFFER,
	GL_PIXEL_PACK_BUFFER,
	GL_QUERY_BUFFER,
	GL_TEXTURE_BUFFER,
	GL_TRANSFORM_FEEDBACK_BUFFER,
	GL_UNIFORM_BUFFER,
	GL_DRAW_INDIRECT_BUFFER,
	GL_ATOMIC_COUNTER_BUFFER,
	GL_DISPATCH_INDIRECT_BUFFER,
	GL_SHADER_STORAGE_BUFFER,
	GL_PARAMETER_BUFFER
};

constexpr static uint32_t GLBufferAccess[] = {
	GL_READ_ONLY,
	GL_WRITE_ONLY,
	GL_READ_WRITE
};

void GL46_Buffer::Init(BufferType type)
{
	m_type = type;
	glGenBuffers( 1, &m_bufferID );
	glBindBuffer( GLBufferType[(uint8_t)type], m_bufferID );
}


void GL46_Buffer::Bind()
{
	if (m_bufferID == 0)
	{
		//DebugLog(LogSeverity::INFO, L"Tried to use Bind() on a non-initialised buffer.");
		return;
	}
	glBindBuffer( GLBufferType[(uint8_t)m_type], m_bufferID );
}


void GL46_Buffer::BindBase( uint32_t base )
{
	if (m_bufferID == 0)
	{
		//DebugLog(LogSeverity::INFO, L"Tried to use BindBase() on a non-initialised buffer.");
		return;
	}
	glBindBufferBase(GLBufferType[(uint8_t)m_type], base, m_bufferID);
}


void GL46_Buffer::Clear() const
{
	// Idk if there is a better way to do this
	glDeleteBuffers( 1, &m_bufferID );
}


void GL46_Buffer::Fill( const size_t size, const void* data ) const
{
	if (m_bufferID == 0)
	{
		//DebugLog(LogSeverity::INFO, L"Tried to use FillNone() on a non-initialised buffer.");
		return;
	}
	glNamedBufferStorage( m_bufferID, size, data, GL_DYNAMIC_STORAGE_BIT );
}


void GL46_Buffer::SubData( const size_t offset, const size_t size, const void* data ) const
{
	if (m_bufferID == 0)
	{
		//DebugLog(LogSeverity::INFO, L"Tried to use SubData() on a non-initialised buffer.");
		return;
	}
	glNamedBufferSubData( m_bufferID, offset, size, data );
}


void* GL46_Buffer::Map( BufferAccess access, const size_t offset, const size_t length ) const
{
	if (offset == -1 && length == -1)
	{
		return glMapNamedBuffer(m_bufferID, GLBufferAccess[(uint8_t)access]);
	}
	return glMapNamedBufferRange(m_bufferID, offset, length, GLBufferAccess[(uint8_t)access]);
}


bool GL46_Buffer::Unmap() const
{
	return glUnmapNamedBuffer( m_bufferID );
}

} // Engine