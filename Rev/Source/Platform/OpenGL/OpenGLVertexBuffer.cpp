#include "OpenGLVertexBuffer.h"
#include "Rev/Core/Assert.h"
#include <glad/gl.h>

namespace Rev
{
/*Vertex Buffer*/

FOpenGLVertexBuffer::FOpenGLVertexBuffer(uint32 size)
	: mSize(size)
{
	glCreateBuffers(1, &mHandle);
	glBindBuffer(GL_ARRAY_BUFFER, mHandle);
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

FOpenGLVertexBuffer::FOpenGLVertexBuffer(const float* vertices, uint32 size)
	: mSize(size)
{
	glCreateBuffers(1, &mHandle);
	glBindBuffer(GL_ARRAY_BUFFER, mHandle);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

FOpenGLVertexBuffer::~FOpenGLVertexBuffer()
{
	glDeleteBuffers(1, &mHandle);
}

void FOpenGLVertexBuffer::UpdateSubData(const void* data, uint32 size, uint32 offset)
{
	RE_CORE_ASSERT(size + offset <= mSize);

	glBindBuffer(GL_ARRAY_BUFFER, mHandle);
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

/*Index Buffer*/
FOpenGLIndexBuffer::FOpenGLIndexBuffer(uint32 stride, uint32 count)
	: mStride(stride)
	, mCount(count)
{
	glCreateBuffers(1, &mHandle);
	glBindBuffer(GL_ARRAY_BUFFER, mHandle);
	glBufferData(GL_ARRAY_BUFFER, count * stride, nullptr, GL_DYNAMIC_DRAW);
}

FOpenGLIndexBuffer::FOpenGLIndexBuffer(const void* indices, uint32 stride, uint32 count)
	: mStride(stride)
	, mCount(count)
{
	glCreateBuffers(1, &mHandle);

	// GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
	// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state. 
	glBindBuffer(GL_ARRAY_BUFFER, mHandle);
	glBufferData(GL_ARRAY_BUFFER, count * stride, indices, GL_STATIC_DRAW);
}

FOpenGLIndexBuffer::~FOpenGLIndexBuffer()
{
	glDeleteBuffers(1, &mHandle);
}

void FOpenGLIndexBuffer::UpdateSubData(const void* data, uint32 count, uint32 offset)
{
	RE_CORE_ASSERT(count + offset <= mCount);

	glBindBuffer(GL_ARRAY_BUFFER, mHandle);
	glBufferSubData(GL_ARRAY_BUFFER, mStride * offset, mStride * count, data);
}

/*Vertex Array*/

static GLenum ConvertToOpenGLDataType(EVertexElementType type)
{
	switch (type)
	{
	case EVertexElementType::Float:    return GL_FLOAT;
	case EVertexElementType::Float2:   return GL_FLOAT;
	case EVertexElementType::Float3:   return GL_FLOAT;
	case EVertexElementType::Float4:   return GL_FLOAT;
	case EVertexElementType::Mat3:     return GL_FLOAT;
	case EVertexElementType::Mat4:     return GL_FLOAT;
	case EVertexElementType::Int:      return GL_INT;
	case EVertexElementType::Int2:     return GL_INT;
	case EVertexElementType::Int3:     return GL_INT;
	case EVertexElementType::Int4:     return GL_INT;
	case EVertexElementType::Bool:     return GL_BOOL;
	}

	RE_CORE_ASSERT(false, "Unknown ShaderDataType!");
	return 0;
}

FOpenGLVertexArray::FOpenGLVertexArray()
{
	glCreateVertexArrays(1, &mHandle);
}

FOpenGLVertexArray::~FOpenGLVertexArray()
{
	glDeleteVertexArrays(1, &mHandle);
}

void FOpenGLVertexArray::AddVertexBuffer(const Ref<FRHIVertexBuffer>& InVertexBuffer)
{
	RE_CORE_ASSERT(InVertexBuffer && InVertexBuffer->GetLayout().GetElements().size(), "Unknown vertex buffer layout!");

	glBindVertexArray(mHandle);
	glBindBuffer(GL_ARRAY_BUFFER, *(const GLuint*)InVertexBuffer->GetNativeHandle());

	const auto& layout = InVertexBuffer->GetLayout();
	for (const auto& element : layout)
	{
		switch (element.Type)
		{
		case EVertexElementType::Float:
		case EVertexElementType::Float2:
		case EVertexElementType::Float3:
		case EVertexElementType::Float4:
		{
			glEnableVertexAttribArray(element.Location);
			glVertexAttribPointer(element.Location,
				element.GetComponentCount(),
				ConvertToOpenGLDataType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.Offset);
			break;
		}
		case EVertexElementType::Int:
		case EVertexElementType::Int2:
		case EVertexElementType::Int3:
		case EVertexElementType::Int4:
		case EVertexElementType::Bool:
		{
			glEnableVertexAttribArray(element.Location);
			glVertexAttribIPointer(element.Location,
				element.GetComponentCount(),
				ConvertToOpenGLDataType(element.Type),
				layout.GetStride(),
				(const void*)element.Offset);
			break;
		}
		case EVertexElementType::Mat3:
		case EVertexElementType::Mat4:
		{
			uint8_t count = element.GetComponentCount();
			for (uint8_t i = 0; i < count; i++)
			{
				glEnableVertexAttribArray(element.Location + i);
				glVertexAttribPointer(element.Location + i,
					count,
					ConvertToOpenGLDataType(element.Type),
					element.Normalized ? GL_TRUE : GL_FALSE,
					layout.GetStride(),
					(const void*)(element.Offset + sizeof(float) * count * i));
				glVertexAttribDivisor(element.Location + i, 1);
			}
			break;
		}
		default:
			RE_CORE_ASSERT(false, "Unknown ShaderDataType!");
		}
	}

	mVertexBuffers.push_back(InVertexBuffer);
}

void FOpenGLVertexArray::SetIndexBuffer(const Ref<FRHIIndexBuffer>& InIndexBuffer)
{
	glBindVertexArray(mHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *(const GLuint*)InIndexBuffer->GetNativeHandle());
	mIndexBuffer = InIndexBuffer;
}

}