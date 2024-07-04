#include "VertexBuffer.h"
#include "Rev/Core/Assert.h"

#include "Platform/OpenGL/OpenGLVertexBuffer.h"


namespace Rev
{

uint32 BufferElementTypeSize(EBufferElementType type)
{
	switch (type)
	{
	case EBufferElementType::Float:    return 4;
	case EBufferElementType::Float2:   return 4 * 2;
	case EBufferElementType::Float3:   return 4 * 3;
	case EBufferElementType::Float4:   return 4 * 4;
	case EBufferElementType::Mat3:     return 4 * 3 * 3;
	case EBufferElementType::Mat4:     return 4 * 4 * 4;
	case EBufferElementType::Int:      return 4;
	case EBufferElementType::Int2:     return 4 * 2;
	case EBufferElementType::Int3:     return 4 * 3;
	case EBufferElementType::Int4:     return 4 * 4;
	case EBufferElementType::Bool:     return 1;
	}

	RE_CORE_ASSERT(false, "Unknown BufferElementType!");
	return 0;
}

uint32 BufferElement::GetComponentCount() const
{
	switch (Type)
	{
	case EBufferElementType::Float:   return 1;
	case EBufferElementType::Float2:  return 2;
	case EBufferElementType::Float3:  return 3;
	case EBufferElementType::Float4:  return 4;
	case EBufferElementType::Mat3:    return 3; // 3* float3
	case EBufferElementType::Mat4:    return 4; // 4* float4
	case EBufferElementType::Int:     return 1;
	case EBufferElementType::Int2:    return 2;
	case EBufferElementType::Int3:    return 3;
	case EBufferElementType::Int4:    return 4;
	case EBufferElementType::Bool:    return 1;
	}

	RE_CORE_ASSERT(false, "Unknown ShaderDataType!");
	return 0;
}

Ref<VertexBuffer> VertexBuffer::Create(uint32 size)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    RE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return CreateRef<OpenGLVertexBuffer>(size);
	}

	RE_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

Ref<VertexBuffer> VertexBuffer::Create(const float* vertices, uint32 size)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    RE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return CreateRef<OpenGLVertexBuffer>(vertices, size);
	}

	RE_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

Ref<IndexBuffer> IndexBuffer::Create(const uint32* indices, uint32 size)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    RE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return CreateRef<OpenGLIndexBuffer>(indices, size);
	}

	RE_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

Ref<VertexArray> Rev::VertexArray::Create()
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    RE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return CreateRef<OpenGLVertexArray>();
	}

	RE_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

}