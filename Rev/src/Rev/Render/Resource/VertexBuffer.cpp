#include "VertexBuffer.h"
#include "Rev/Core/Assert.h"

#include "Platform/OpenGL/OpenGLVertexBuffer.h"


namespace Rev
{

uint32 BufferElement::GetComponentCount() const
{
	switch (Type)
	{
	case EShaderDataType::Float:   return 1;
	case EShaderDataType::Float2:  return 2;
	case EShaderDataType::Float3:  return 3;
	case EShaderDataType::Float4:  return 4;
	case EShaderDataType::Mat3:    return 3; // 3* float3
	case EShaderDataType::Mat4:    return 4; // 4* float4
	case EShaderDataType::Int:     return 1;
	case EShaderDataType::Int2:    return 2;
	case EShaderDataType::Int3:    return 3;
	case EShaderDataType::Int4:    return 4;
	case EShaderDataType::Bool:    return 1;
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