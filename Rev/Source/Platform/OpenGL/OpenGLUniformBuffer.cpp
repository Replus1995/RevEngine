#include "OpenGLUniformBuffer.h"
#include <glad/gl.h>

namespace Rev
{

FOpenGLUniformBuffer::FOpenGLUniformBuffer(uint32 size)
{
	glCreateBuffers(1, &mHandle);
	glNamedBufferData(mHandle, size, nullptr, GL_DYNAMIC_DRAW); // TODO: investigate usage hint
}

FOpenGLUniformBuffer::~FOpenGLUniformBuffer()
{
	glDeleteBuffers(1, &mHandle);
}

void FOpenGLUniformBuffer::UpdateSubData(const void* data, uint32 size, uint32 offset)
{
	glNamedBufferSubData(mHandle, offset, size, data);
}

}