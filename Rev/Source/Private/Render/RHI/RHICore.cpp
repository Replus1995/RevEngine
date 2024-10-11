#include "Rev/Render/RHI/RHICore.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Core/Assert.h"

//Vulkan impl headers
#include "VulkanRHI/VulkanCore.h"
#include "VulkanRHI/VulkanContext.h"
#include "VulkanRHI/VulkanBuffer.h"
#include "VulkanRHI/VulkanUniform.h"
#include "VulkanRHI/VulkanSampler.h"
#include "VulkanRHI/VulkanTexture.h"
#include "VulkanRHI/VulkanRenderTarget.h"
#include "VulkanRHI/VulkanPrimitive.h"
#include "VulkanRHI/VulkanRenderPass.h"

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
	case ERenderAPI::Vulkan:  return CreateRef<FVulkanVertexBuffer>(InSize, InVertices);
	}

	REV_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

Ref<FRHIIndexBuffer> FRHICore::CreateIndexBuffer(EIndexElementType InType, uint32 InCount)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    REV_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::Vulkan:  return CreateRef<FVulkanIndexBuffer>(InType, InCount);
	}

	REV_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

Ref<FRHIIndexBuffer> FRHICore::CreateIndexBuffer(const void* InIndices, EIndexElementType InType, uint32 InCount)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    REV_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::Vulkan:  return CreateRef<FVulkanIndexBuffer>(InType, InCount, InIndices);
	}

	REV_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

Ref<FRHIUniformBuffer> FRHICore::CreateUniformBuffer(uint32 InSize)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    REV_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::Vulkan:  return CreateRef<FVulkanUniformBuffer>(InSize);
	}

	REV_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

Ref<FRHIUniformBufferDynamic> FRHICore::CreateUniformBufferDynamic(uint32 InSize)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    REV_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::Vulkan:  return CreateRef<FVulkanUniformBufferDynamic>(InSize);
	}

	REV_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

Ref<FRHIPrimitive> FRHICore::CreatePrimitive(EPrimitiveTopology InTopology)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    REV_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::Vulkan:  return CreateRef<FVulkanPrimitive>(InTopology);
	}

	REV_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}

Ref<FRHITexture> FRHICore::CreateTexture(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    REV_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
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
	case ERenderAPI::Vulkan:  return CreateRef<FVulkanRenderTarget>(InDesc);
	}

	REV_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}
Ref<FRHIRenderPass> FRHICore::CreateRenderPass(const FRenderPassDesc& InDesc)
{
	switch (GetRenderAPI())
	{
	case ERenderAPI::None:    REV_CORE_ASSERT(false, "ERenderAPI::None is currently not supported!"); return nullptr;
	case ERenderAPI::Vulkan:  return CreateRef<FVulkanRenderPass>(InDesc);
	}

	REV_CORE_ASSERT(false, "Unknown RenderAPI!");
	return nullptr;
}
}