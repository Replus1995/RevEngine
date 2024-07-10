#include "OpenGLUniformBuffer.h"
#include <glad/glad.h>

namespace Rev
{

FOpenGLUniformBuffer::FOpenGLUniformBuffer(uint32_t size, uint32_t binding)
{
	glCreateBuffers(1, &mHandle);
	glNamedBufferData(mHandle, size, nullptr, GL_DYNAMIC_DRAW); // TODO: investigate usage hint
	glBindBufferBase(GL_UNIFORM_BUFFER, binding, mHandle);
}

FOpenGLUniformBuffer::~FOpenGLUniformBuffer()
{
	glDeleteBuffers(1, &mHandle);
}

void FOpenGLUniformBuffer::UpdateData(const void* data, uint32_t size, uint32_t offset)
{
	glNamedBufferSubData(mHandle, offset, size, data);
}

}