#include "pinepch.h"
#include "VertexBuffer.h"
#include "Pine/Core/Assert.h"

#include "Platform/OpenGL/OpenGLVertexBuffer.h"


namespace Pine
{

uint32_t BufferElement::GetComponentCount() const
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

	PE_CORE_ASSERT(false, "Unknown ShaderDataType!");
	return 0;
}

Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    PE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return CreateRef<OpenGLVertexBuffer>(size);
	}

	PE_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    PE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return CreateRef<OpenGLVertexBuffer>(vertices, size);
	}

	PE_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t size)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    PE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return CreateRef<OpenGLIndexBuffer>(indices, size);
	}

	PE_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

Ref<VertexArray> Pine::VertexArray::Create()
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    PE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return CreateRef<OpenGLVertexArray>();
	}

	PE_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

}