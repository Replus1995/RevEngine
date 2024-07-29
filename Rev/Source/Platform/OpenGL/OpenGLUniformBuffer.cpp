#include "OpenGLUniformBuffer.h"
#include <glad/gl.h>

namespace Rev
{

FOpenGLUniformBuffer::FOpenGLUniformBuffer(uint32 size, uint32 binding)
{
	glCreateBuffers(1, &mHandle);
	glNamedBufferData(mHandle, size, nullptr, GL_DYNAMIC_DRAW); // TODO: investigate usage hint
	glBindBufferBase(GL_UNIFORM_BUFFER, binding, mHandle);
}

FOpenGLUniformBuffer::~FOpenGLUniformBuffer()
{
	glDeleteBuffers(1, &mHandle);
}

void FOpenGLUniformBuffer::UpdateLayerData(const void* data, uint32 size, uint32 offset)
{
	glNamedBufferSubData(mHandle, offset, size, data);
}

}