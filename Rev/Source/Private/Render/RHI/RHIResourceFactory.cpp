#include "Rev/Render/RHI/RHIResourceFactory.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Core/Assert.h"

//OpenGL impl headers
#include "OpenGL/OpenGLVertexBuffer.h"
#include "OpenGL/OpenGLUniformBuffer.h"
#include "OpenGL/OpenGLSampler.h"
#include "OpenGL/OpenGLTexture.h"
#include "OpenGL/OpenGLRenderTarget2D.h"

namespace Rev
{

namespace
{

template<class TRHIResource, typename... Args>
Ref<TRHIResource> CreateRHIResource(Args&&... args)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    RE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return CreateRef<TRHIResource>(std::forward<Args>(args)...);
	}

	RE_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

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

Ref<FRHIVertexArray> FRHIResourceFactory::CreateVertexData()
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

//Ref<FRHISampler> FRHIResourceFactory::CreateSampler(const FSamplerDesc& InDesc)
//{
//	switch (GetRenderAPI())
//	{
//	case ERenderAPI::None:    RE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
//	case ERenderAPI::OpenGL:  return CreateRef<FOpenGLSampler>(InDesc);
//	}
//
//	RE_CORE_ASSERT(false, "Unknown RenderAPI!");
//	return nullptr;
//}


Ref<FRHITexture> FRHIResourceFactory::CreateTexture(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    RE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return FOpenGLTexture::Create(InDesc, InSamplerDesc);
	}

	RE_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

Ref<FRHIRenderTarget> FRHIResourceFactory::CreateRenderTarget(const FRenderTargetDesc& InDesc)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    RE_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return CreateRef<FOpenGLRenderTarget2D>(InDesc);
	}

	RE_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}
}