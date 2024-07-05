#include "Rev/Render/Resource/VertexBuffer.h"
#include "Rev/Core/Assert.h"

#include "Platform/OpenGL/OpenGLVertexBuffer.h"


namespace Rev
{

uint32 VertexElementSize(EVertexElementType type)
{
	switch (type)
	{
	case EVertexElementType::Float:    return 4;
	case EVertexElementType::Float2:   return 4 * 2;
	case EVertexElementType::Float3:   return 4 * 3;
	case EVertexElementType::Float4:   return 4 * 4;
	case EVertexElementType::Mat3:     return 4 * 3 * 3;
	case EVertexElementType::Mat4:     return 4 * 4 * 4;
	case EVertexElementType::Int:      return 4;
	case EVertexElementType::Int2:     return 4 * 2;
	case EVertexElementType::Int3:     return 4 * 3;
	case EVertexElementType::Int4:     return 4 * 4;
	case EVertexElementType::Bool:     return 1;
	}

	RE_CORE_ASSERT(false, "Unknown BufferElementType!");
	return 0;
}

uint32 FVertexBufferElement::GetComponentCount() const
{
	switch (Type)
	{
	case EVertexElementType::Float:   return 1;
	case EVertexElementType::Float2:  return 2;
	case EVertexElementType::Float3:  return 3;
	case EVertexElementType::Float4:  return 4;
	case EVertexElementType::Mat3:    return 3; // 3* float3
	case EVertexElementType::Mat4:    return 4; // 4* float4
	case EVertexElementType::Int:     return 1;
	case EVertexElementType::Int2:    return 2;
	case EVertexElementType::Int3:    return 3;
	case EVertexElementType::Int4:    return 4;
	case EVertexElementType::Bool:    return 1;
	}

	RE_CORE_ASSERT(false, "Unknown ShaderDataType!");
	return 0;
}

Ref<FVertexBuffer> FVertexBuffer::Create(uint32 size)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    RE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return CreateRef<FOpenGLVertexBuffer>(size);
	}

	RE_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

Ref<FVertexBuffer> FVertexBuffer::Create(const float* vertices, uint32 size)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    RE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return CreateRef<FOpenGLVertexBuffer>(vertices, size);
	}

	RE_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

Ref<FIndexBuffer> FIndexBuffer::Create(uint32 stride, uint32 count)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    RE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return CreateRef<FOpenGLIndexBuffer>(stride, count);
	}

	RE_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

Ref<FIndexBuffer> FIndexBuffer::Create(const void* indices, uint32 stride, uint32 count)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    RE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return CreateRef<FOpenGLIndexBuffer>(indices, stride, count);
	}

	RE_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

Ref<FVertexArray> Rev::FVertexArray::Create()
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    RE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return CreateRef<FOpenGLVertexArray>();
	}

	RE_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

}