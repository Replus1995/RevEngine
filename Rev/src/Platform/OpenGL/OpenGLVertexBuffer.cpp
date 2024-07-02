#include "pinepch.h"
#include "OpenGLVertexBuffer.h"
#include "Pine/Core/Assert.h"

#include <glad/glad.h>

namespace Pine
{
/*Vertex Buffer*/

OpenGLVertexBuffer::OpenGLVertexBuffer(uint32 size)
{
	glCreateBuffers(1, &mHandle);
	glBindBuffer(GL_ARRAY_BUFFER, mHandle);
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

OpenGLVertexBuffer::OpenGLVertexBuffer(const float* vertices, uint32 size)
{
	glCreateBuffers(1, &mHandle);
	glBindBuffer(GL_ARRAY_BUFFER, mHandle);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
	glDeleteBuffers(1, &mHandle);
}

void OpenGLVertexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, mHandle);
}

void OpenGLVertexBuffer::Unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLVertexBuffer::SetData(const void* data, uint32 size)
{
	glBindBuffer(GL_ARRAY_BUFFER, mHandle);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

/*Index Buffer*/

OpenGLIndexBuffer::OpenGLIndexBuffer(const uint32* indices, uint32 count)
	: mCount(count)
{
	glCreateBuffers(1, &mHandle);

	// GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
	// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state. 
	glBindBuffer(GL_ARRAY_BUFFER, mHandle);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32), indices, GL_STATIC_DRAW);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
	glDeleteBuffers(1, &mHandle);
}

void OpenGLIndexBuffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mHandle);
}

void OpenGLIndexBuffer::Unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

/*Vertex Array*/

static GLenum ConvertToOpenGLDataType(EShaderDataType type)
{
	switch (type)
	{
	case EShaderDataType::Float:    return GL_FLOAT;
	case EShaderDataType::Float2:   return GL_FLOAT;
	case EShaderDataType::Float3:   return GL_FLOAT;
	case EShaderDataType::Float4:   return GL_FLOAT;
	case EShaderDataType::Mat3:     return GL_FLOAT;
	case EShaderDataType::Mat4:     return GL_FLOAT;
	case EShaderDataType::Int:      return GL_INT;
	case EShaderDataType::Int2:     return GL_INT;
	case EShaderDataType::Int3:     return GL_INT;
	case EShaderDataType::Int4:     return GL_INT;
	case EShaderDataType::Bool:     return GL_BOOL;
	}

	PE_CORE_ASSERT(false, "Unknown ShaderDataType!");
	return 0;
}

OpenGLVertexArray::OpenGLVertexArray()
{
	glCreateVertexArrays(1, &mHandle);
}

OpenGLVertexArray::~OpenGLVertexArray()
{
	glDeleteVertexArrays(1, &mHandle);
}

void OpenGLVertexArray::Bind() const
{
	glBindVertexArray(mHandle);
}

void OpenGLVertexArray::Unbind() const
{
	glBindVertexArray(0);
}

void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
{
	PE_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Unknown vertex buffer layout!");

	glBindVertexArray(mHandle);
	vertexBuffer->Bind();

	const auto& layout = vertexBuffer->GetLayout();
	for (const auto& element : layout)
	{
		switch (element.Type)
		{
		case EShaderDataType::Float:
		case EShaderDataType::Float2:
		case EShaderDataType::Float3:
		case EShaderDataType::Float4:
		{
			glEnableVertexAttribArray(mVertexBufferIndex);
			glVertexAttribPointer(mVertexBufferIndex,
				element.GetComponentCount(),
				ConvertToOpenGLDataType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.Offset);
			mVertexBufferIndex++;
			break;
		}
		case EShaderDataType::Int:
		case EShaderDataType::Int2:
		case EShaderDataType::Int3:
		case EShaderDataType::Int4:
		case EShaderDataType::Bool:
		{
			glEnableVertexAttribArray(mVertexBufferIndex);
			glVertexAttribIPointer(mVertexBufferIndex,
				element.GetComponentCount(),
				ConvertToOpenGLDataType(element.Type),
				layout.GetStride(),
				(const void*)element.Offset);
			mVertexBufferIndex++;
			break;
		}
		case EShaderDataType::Mat3:
		case EShaderDataType::Mat4:
		{
			uint8_t count = element.GetComponentCount();
			for (uint8_t i = 0; i < count; i++)
			{
				glEnableVertexAttribArray(mVertexBufferIndex);
				glVertexAttribPointer(mVertexBufferIndex,
					count,
					ConvertToOpenGLDataType(element.Type),
					element.Normalized ? GL_TRUE : GL_FALSE,
					layout.GetStride(),
					(const void*)(element.Offset + sizeof(float) * count * i));
				glVertexAttribDivisor(mVertexBufferIndex, 1);
				mVertexBufferIndex++;
			}
			break;
		}
		default:
			PE_CORE_ASSERT(false, "Unknown ShaderDataType!");
		}
	}

	mVertexBuffers.push_back(vertexBuffer);
}

void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
{
	glBindVertexArray(mHandle);
	indexBuffer->Bind();
	mIndexBuffer = indexBuffer;
}

}