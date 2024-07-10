#include "Rev/Render/RHI/RHIResourceFactory.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Core/Assert.h"

//OpenGL impl headers
#include "Platform/OpenGL/OpenGLVertexBuffer.h"
#include "Platform/OpenGL/OpenGLUniformBuffer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Rev
{

namespace OpenGLImpl
{

}


Ref<FRHIVertexBuffer> FRHIResourceFactory::CreateVertexBuffer(uint32 InSize)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    RE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return CreateRef<FOpenGLVertexBuffer>(InSize);
	}

	RE_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

Ref<FRHIVertexBuffer> FRHIResourceFactory::CreateVertexBuffer(const float* InVertices, uint32 InSize)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    RE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return CreateRef<FOpenGLVertexBuffer>(InVertices, InSize);
	}

	RE_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

Ref<FRHIIndexBuffer> FRHIResourceFactory::CreateIndexBuffer(uint32 InStride, uint32 InCount)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    RE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return CreateRef<FOpenGLIndexBuffer>(InStride, InCount);
	}

	RE_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

Ref<FRHIIndexBuffer> FRHIResourceFactory::CreateIndexBuffer(const void* InIndices, uint32 InStride, uint32 InCount)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    RE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return CreateRef<FOpenGLIndexBuffer>(InIndices, InStride, InCount);
	}

	RE_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

Ref<FRHIVertexArray> FRHIResourceFactory::CreateVertexArray()
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    RE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return CreateRef<FOpenGLVertexArray>();
	}

	RE_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

Ref<FRHIUniformBuffer> FRHIResourceFactory::CreateUniformBuffer(uint32 InSize, uint32 InBinding)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    RE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return CreateRef<FOpenGLUniformBuffer>(InSize, InBinding);
	}

	RE_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

Ref<FRHITexture> FRHIResourceFactory::CreateTexture(const FTextureDesc& InDesc)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    RE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return FOpenGLTexture::Create(InDesc);
	}

	RE_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

Ref<FRHISamplerState> FRHIResourceFactory::CreateSampler(const FSamplerDesc& InDesc)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    RE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return CreateRef<FOpenGLSampler>(InDesc);
	}

	RE_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

}