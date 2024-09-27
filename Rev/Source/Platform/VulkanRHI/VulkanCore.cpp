#include "VulkanCore.h"
#include "VulkanUniform.h"
#include "Rev/Core/Assert.h"
#include "Rev/Render/RenderCmd.h"

namespace Rev
{

struct FVulkanCorePrivate
{
public:
	FVulkanInstance Instance;
	FVulkanDevice Device;
	VmaAllocator Allocator = nullptr;
	FVulkanUniformManager* UniformManager = nullptr;


	FVulkanCorePrivate()
	{
		Instance.CreateInstance();
		Instance.CreateSurface();
		Device.PickPhysicalDevice(&Instance);
		Device.CreateLogicalDevice(&Instance);
		
		VmaAllocatorCreateInfo AllocatorCreateInfo = {};
		AllocatorCreateInfo.physicalDevice = Device.GetPhysicalDevice();
		AllocatorCreateInfo.device = Device.GetDevice();
		AllocatorCreateInfo.instance = Instance.GetInstance();
		AllocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
		vmaCreateAllocator(&AllocatorCreateInfo, &Allocator);

		UniformManager = new FVulkanUniformManager();
	}

	~FVulkanCorePrivate()
	{
		delete UniformManager;

		vmaDestroyAllocator(Allocator);
		Device.Cleanup();
		Instance.Cleanup();
	}
};
static std::unique_ptr<FVulkanCorePrivate> sVulkanCore;


void FVulkanCore::Init()
{
	if(!sVulkanCore)
		sVulkanCore = std::make_unique<FVulkanCorePrivate>();
}

void FVulkanCore::Cleanup()
{
	sVulkanCore.reset();
}

VkInstance FVulkanCore::GetInstance()
{
	return sVulkanCore->Instance.GetInstance();
}

VkSurfaceKHR FVulkanCore::GetSurface()
{
	return sVulkanCore->Instance.GetSurface();
}

VkPhysicalDevice FVulkanCore::GetPhysicalDevice()
{
	return sVulkanCore->Device.GetPhysicalDevice();
}

VkDevice FVulkanCore::GetDevice()
{
	return sVulkanCore->Device.GetDevice();
}

VkQueue FVulkanCore::GetQueue(EVulkanQueueKind InKind)
{
	return sVulkanCore->Device.GetQueue(InKind);
}

uint32 FVulkanCore::GetQueueFamily(EVulkanQueueKind InKind)
{
	return sVulkanCore->Device.GetQueueFamily(InKind);
}

const FVulkanSwapChainSupport& FVulkanCore::GetSwapChainSupport()
{
	return sVulkanCore->Device.GetSwapChainSupport();
}

VmaAllocator FVulkanCore::GetAllocator()
{
	return sVulkanCore->Allocator;
}

FVulkanContext* FVulkanCore::GetContext()
{
	REV_CORE_ASSERT(GetRenderAPI() == ERenderAPI::Vulkan);
	return static_cast<FVulkanContext*>(RenderCmd::GetContext());
}

VkCommandBuffer FVulkanCore::GetMainCmdBuffer()
{
	return GetContext()->GetMainCmdBuffer();
}

VkExtent2D FVulkanCore::GetSwapchainExtent()
{
	return GetContext()->GetSwapchain().GetExtent();
}

void FVulkanCore::ImmediateSubmit(std::function<void(VkCommandBuffer)>&& Func)
{
	GetContext()->ImmediateSubmit(std::move(Func));
}

FVulkanUniformManager* FVulkanCore::GetUniformManager()
{
	return sVulkanCore->UniformManager;
}


}