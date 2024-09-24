#include "Rev/Render/RHI/RHICore.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Core/Assert.h"

//OpenGL impl headers
#include "OpenGL/OpenGLContext.h"
#include "OpenGL/OpenGLBuffer.h"
#include "OpenGL/OpenGLSampler.h"
#include "OpenGL/OpenGLTexture.h"
#include "OpenGL/OpenGLRenderTarget.h"

//Vulkan impl headers
#include "VulkanRHI/VulkanCore.h"
#include "VulkanRHI/VulkanContext.h"
#include "VulkanRHI/VulkanBuffer.h"
#include "VulkanRHI/VulkanSampler.h"
#include "VulkanRHI/VulkanTexture.h"
#include "VulkanRHI/VulkanRenderTarget.h"

namespace Rev
{

//namespace
//{
//
//template<class TRHIResource, typename... Args>
//Ref<TRHIResource> CreateRHIResource(Args&&... args)
//{
//	switch (GetRenderAPI())
//	{
//	case ERenderAPI::None:    REV_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
//	case ERenderAPI::OpenGL:  return CreateRef<TRHIResource>(std::forward<Args>(args)...);
//	}
//
//	REV_CORE_ASSERT(false, "Unknown RenderAPI!");
//	return nullptr;
//}
//
//}

void FRHICore::Init()
{
	FVulkanCore::Init();
}

void FRHICore::Cleanup()
{
	FVulkanCore::Cleanup();
}

Scope<FRHIContext> FRHICore::CreateContext()
{
	switch (GetRenderAPI())
	{
	case Rev::ERenderAPI::None:
		REV_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!");
		return nullptr;
	case Rev::ERenderAPI::OpenGL:
		return CreateScope<FOpenGLContext>();
	case Rev::ERenderAPI::Vulkan:
		return CreateScope<FVulkanContext>();
	}

	REV_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

Ref<FRHIVertexBuffer> FRHICore::CreateVertexBuffer(uint32 InSize)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    REV_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return CreateRef<FOpenGLVertexBuffer>(InSize);
	case ERenderAPI::Vulkan:  return CreateRef<FVulkanVertexBuffer>(InSize);
	}

	REV_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

Ref<FRHIVertexBuffer> FRHICore::CreateVertexBuffer(const float* InVertices, uint32 InSize)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    REV_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return CreateRef<FOpenGLVertexBuffer>(InSize, InVertices);
	case ERenderAPI::Vulkan:  return CreateRef<FVulkanVertexBuffer>(InSize, InVertices);
	}

	REV_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

Ref<FRHIIndexBuffer> FRHICore::CreateIndexBuffer(uint32 InStride, uint32 InCount)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    REV_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return CreateRef<FOpenGLIndexBuffer>(InStride, InCount);
	case ERenderAPI::Vulkan:  return CreateRef<FVulkanIndexBuffer>(InStride, InCount);
	}

	REV_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

Ref<FRHIIndexBuffer> FRHICore::CreateIndexBuffer(const void* InIndices, uint32 InStride, uint32 InCount)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    REV_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return CreateRef<FOpenGLIndexBuffer>(InStride, InCount, InIndices);
	case ERenderAPI::Vulkan:  return CreateRef<FVulkanIndexBuffer>(InStride, InCount, InIndices);
	}

	REV_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

Ref<FRHIVertexArray> FRHICore::CreateVertexArray()
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    REV_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return CreateRef<FOpenGLVertexArray>();
	}

	REV_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

Ref<FRHIUniformBuffer> FRHICore::CreateUniformBuffer(uint32 InSize)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    REV_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return CreateRef<FOpenGLUniformBuffer>(InSize);
	case ERenderAPI::Vulkan:  return CreateRef<FVulkanUniformBuffer>(InSize);
	}

	REV_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

Ref<FRHISampler> FRHICore::CreateSampler(const FSamplerDesc& InDesc)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    REV_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	//case ERenderAPI::OpenGL:  return CreateRef<FOpenGLSampler>(InDesc);
	case ERenderAPI::Vulkan:  return CreateRef<FVulkanSampler>(InDesc);
	}

	REV_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}


Ref<FRHITexture> FRHICore::CreateTexture(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    REV_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return CreateOpenGLTexture(InDesc, InSamplerDesc);
	case ERenderAPI::Vulkan:  return CreateVulkanTexture(InDesc, InSamplerDesc);
	}

	REV_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

Ref<FRHIRenderTarget> FRHICore::CreateRenderTarget(const FRenderTargetDesc& InDesc)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    REV_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::OpenGL:  return CreateRef<FOpenGLRenderTarget>(InDesc);
	case ERenderAPI::Vulkan:  return CreateRef<FVulkanRenderTarget>(InDesc);
	}

	REV_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}
}