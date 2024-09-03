#include "VkSwapChain.h"
#include "Rev/Core/Assert.h"
#include "Rev/Core/Application.h"
#include "Rev/Core/Window.h"
#include <set>
#include <limits>
#include <algorithm>
#include "VkDefines.h"
#include "VkInstance.h"
#include "VkDevice.h"
#include "../VkInitializer.h"

namespace Rev
{

void FVulkanSwapchain::CreateSwapchain(const FVulkanInstance* Instance, const FVulkanDevice* InDevice, VmaAllocator InAllocator)
{
    REV_CORE_ASSERT(Instance);
    REV_CORE_ASSERT(InDevice);
    REV_CORE_ASSERT(InAllocator);

    const FVulkanDeviceSwapChainSupport& SwapChainSupport = InDevice->GetSwapChainSupport();

    VkSurfaceFormatKHR SurfaceFormat = ChooseSurfaceFormat(SwapChainSupport.Formats);
    VkPresentModeKHR PresentMode = ChoosePresentMode(SwapChainSupport.PresentModes);
    VkExtent2D Extent = ChooseExtent(SwapChainSupport.Capabilities);

    uint32 ImageCount = SwapChainSupport.Capabilities.minImageCount + 1;
    if (SwapChainSupport.Capabilities.maxImageCount > 0 && ImageCount > SwapChainSupport.Capabilities.maxImageCount) {
        ImageCount = SwapChainSupport.Capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR SwapChainCreateInfo{};
    SwapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    SwapChainCreateInfo.surface = Instance->GetSurface();
    SwapChainCreateInfo.minImageCount = ImageCount;
    SwapChainCreateInfo.imageFormat = SurfaceFormat.format;
    SwapChainCreateInfo.imageColorSpace = SurfaceFormat.colorSpace;
    SwapChainCreateInfo.imageExtent = Extent;
    SwapChainCreateInfo.imageArrayLayers = 1;
    SwapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    {
        //queue families
        std::set<uint32> UniqueIndexSet;
        UniqueIndexSet.insert(InDevice->GetQueueFamily(VQK_Present));
        UniqueIndexSet.insert(InDevice->GetQueueFamily(VQK_Graphics));

        std::vector<uint32> UniqueIndices;
        for (uint32 Index : UniqueIndexSet)
        {
            UniqueIndices.push_back(Index);
        }

        SwapChainCreateInfo.imageSharingMode = UniqueIndices.size() > 1 ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
        SwapChainCreateInfo.queueFamilyIndexCount = UniqueIndices.size() > 1 ? UniqueIndices.size() : 0;
        SwapChainCreateInfo.pQueueFamilyIndices = UniqueIndices.size() > 1 ? UniqueIndices.data() : nullptr;

    }
    
    SwapChainCreateInfo.preTransform = SwapChainSupport.Capabilities.currentTransform;
    SwapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    SwapChainCreateInfo.presentMode = PresentMode;
    SwapChainCreateInfo.clipped = VK_TRUE;
    SwapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    REV_VK_CHECK_THROW(vkCreateSwapchainKHR(InDevice->GetLogicalDevice(), &SwapChainCreateInfo, nullptr, &mSwapchain), "[FVkSwapChain] Failed to create swap chain!");

    mExtent = Extent;
    mFormat = SurfaceFormat.format;
    vkGetSwapchainImagesKHR(InDevice->GetLogicalDevice(), mSwapchain, &ImageCount, nullptr);
    mImages.resize(ImageCount);
    vkGetSwapchainImagesKHR(InDevice->GetLogicalDevice(), mSwapchain, &ImageCount, mImages.data());
    CreateImageViews(InDevice);

    CreateBackImage(InDevice, InAllocator);
}

void FVulkanSwapchain::Cleanup(const FVulkanDevice* InDevice, VmaAllocator InAllocator)
{
    REV_CORE_ASSERT(InDevice);
    REV_CORE_ASSERT(InAllocator);

    vkDestroyImageView(InDevice->GetLogicalDevice(), mBackImage.ImageView, nullptr);
    vmaDestroyImage(InAllocator, mBackImage.Image, mBackImage.Allocation);

    for (auto ImageView : mImageViews) {
        vkDestroyImageView(InDevice->GetLogicalDevice(), ImageView, nullptr);
    }
    mImageViews.clear();
    mImages.clear();
    vkDestroySwapchainKHR(InDevice->GetLogicalDevice(), mSwapchain, nullptr);
}

VkSurfaceFormatKHR FVulkanSwapchain::ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& InAvailableFormats)
{
    for (const auto& AvailableFormat : InAvailableFormats) {
        if (AvailableFormat.format == VK_FORMAT_R8G8B8A8_UNORM && AvailableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return AvailableFormat;
        }
    }
    return InAvailableFormats[0];
}

VkPresentModeKHR FVulkanSwapchain::ChoosePresentMode(const std::vector<VkPresentModeKHR>& InAvailablePresentModes)
{
    for (const auto& AvailablePresentMode : InAvailablePresentModes) {
        if (AvailablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return AvailablePresentMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D FVulkanSwapchain::ChooseExtent(const VkSurfaceCapabilitiesKHR& InCapabilities)
{
    if (InCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return InCapabilities.currentExtent;
    }
    else {
        Window* pWnd = Application::GetApp().GetWindow();
        REV_CORE_ASSERT(pWnd, "[FVkSwapChain] Invalid window!");

        int32 FrameWidth, FrameHeight;
        pWnd->GetFrameSize(FrameWidth, FrameHeight);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(FrameWidth),
            static_cast<uint32_t>(FrameHeight)
        };

        actualExtent.width = std::clamp(actualExtent.width, InCapabilities.minImageExtent.width, InCapabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, InCapabilities.minImageExtent.height, InCapabilities.maxImageExtent.height);

        return actualExtent;
    }
}

void FVulkanSwapchain::CreateImageViews(const FVulkanDevice* InDevice)
{
    REV_CORE_ASSERT(InDevice);

    mImageViews.resize(mImages.size());
    for (size_t i = 0; i < mImages.size(); i++)
    {
        VkImageViewCreateInfo ImageViewCreateInfo{};
        ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ImageViewCreateInfo.image = mImages[i];
        ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        ImageViewCreateInfo.format = mFormat;
        ImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        ImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        ImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        ImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        ImageViewCreateInfo.subresourceRange.levelCount = 1;
        ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        ImageViewCreateInfo.subresourceRange.layerCount = 1;

        REV_VK_CHECK_THROW(vkCreateImageView(InDevice->GetLogicalDevice(), &ImageViewCreateInfo, nullptr, &mImageViews[i]), "[FVkSwapChain] Failed to create image views!");
    }
}

void FVulkanSwapchain::CreateBackImage(const FVulkanDevice* InDevice, VmaAllocator InAllocator)
{
    REV_CORE_ASSERT(InDevice);
    REV_CORE_ASSERT(InAllocator);

    VkExtent3D BackImageExtent = {
        mExtent.width,
        mExtent.height,
        1
    };

    //hardcoding the draw format to 32 bit float
    mBackImage.Format = VK_FORMAT_R16G16B16A16_SFLOAT;
    mBackImage.Extent = BackImageExtent;

    VkImageUsageFlags BackImageUsages{};
    BackImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    BackImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    BackImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
    BackImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    VkImageCreateInfo ImageCreateInfo = FVulkanInit::ImageCreateInfo2D(mBackImage.Format, BackImageUsages, BackImageExtent);

    //for the draw image, we want to allocate it from gpu local memory
    VmaAllocationCreateInfo ImageAllocinfo = {};
    ImageAllocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    ImageAllocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    //allocate and create the image
    vmaCreateImage(InAllocator, &ImageCreateInfo, &ImageAllocinfo, &mBackImage.Image, &mBackImage.Allocation, nullptr);

    //build a image-view for the draw image to use for rendering
    VkImageViewCreateInfo ImageViewCreateInfo = FVulkanInit::ImageViewCreateInfo2D(mBackImage.Format, mBackImage.Image, VK_IMAGE_ASPECT_COLOR_BIT);

    REV_VK_CHECK(vkCreateImageView(InDevice->GetLogicalDevice(), &ImageViewCreateInfo, nullptr, &mBackImage.ImageView));

}

}


