#include "VulkanSwapChain.h"
#include "VulkanDynamicRHI.h"
#include "Core/VulkanDefines.h"
#include "Rev/Core/Assert.h"
#include "Rev/Core/Application.h"
#include "Rev/Core/Window.h"
#include <set>
#include <limits>
#include <algorithm>

namespace Rev
{

void FVulkanSwapchain::CreateSwapchain(VkPresentModeKHR InPresentMode)
{
    const FVulkanSurfaceSupport& SurfaceSupport = FVulkanDynamicRHI::GetSurfaceSupport();

    VkSurfaceFormatKHR SurfaceFormat = ChooseSurfaceFormat(SurfaceSupport.Formats);
    VkPresentModeKHR PresentMode = ChoosePresentMode(SurfaceSupport.PresentModes, InPresentMode);
    VkExtent2D Extent = ChooseExtent(SurfaceSupport.Capabilities);

    uint32 ImageCount = SurfaceSupport.Capabilities.minImageCount + 1;
    if (SurfaceSupport.Capabilities.maxImageCount > 0 && ImageCount > SurfaceSupport.Capabilities.maxImageCount) {
        ImageCount = SurfaceSupport.Capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR SwapChainCreateInfo{};
    SwapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    SwapChainCreateInfo.surface = FVulkanDynamicRHI::GetSurface();
    SwapChainCreateInfo.minImageCount = ImageCount;
    SwapChainCreateInfo.imageFormat = SurfaceFormat.format;
    SwapChainCreateInfo.imageColorSpace = SurfaceFormat.colorSpace;
    SwapChainCreateInfo.imageExtent = Extent;
    SwapChainCreateInfo.imageArrayLayers = 1;
    SwapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    {
        //queue families
        std::set<uint32> UniqueIndexSet;
        UniqueIndexSet.insert(FVulkanDynamicRHI::GetQueueFamily(VQK_Present));
        UniqueIndexSet.insert(FVulkanDynamicRHI::GetQueueFamily(VQK_Graphics));

        std::vector<uint32> UniqueIndices;
        for (uint32 Index : UniqueIndexSet)
        {
            UniqueIndices.push_back(Index);
        }

        SwapChainCreateInfo.imageSharingMode = UniqueIndices.size() > 1 ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
        SwapChainCreateInfo.queueFamilyIndexCount = UniqueIndices.size() > 1 ? UniqueIndices.size() : 0;
        SwapChainCreateInfo.pQueueFamilyIndices = UniqueIndices.size() > 1 ? UniqueIndices.data() : nullptr;

    }
    
    SwapChainCreateInfo.preTransform = SurfaceSupport.Capabilities.currentTransform;
    SwapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    SwapChainCreateInfo.presentMode = PresentMode;
    SwapChainCreateInfo.clipped = VK_TRUE;
    SwapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    REV_VK_CHECK_THROW(vkCreateSwapchainKHR(FVulkanDynamicRHI::GetDevice(), &SwapChainCreateInfo, nullptr, &mSwapchain), "[FVkSwapChain] Failed to create swap chain!");

    mExtent = Extent;
    mFormat = SurfaceFormat.format;
    vkGetSwapchainImagesKHR(FVulkanDynamicRHI::GetDevice(), mSwapchain, &ImageCount, nullptr);
    mImages.resize(ImageCount);
    vkGetSwapchainImagesKHR(FVulkanDynamicRHI::GetDevice(), mSwapchain, &ImageCount, mImages.data());
    CreateImageViews();
}

void FVulkanSwapchain::Cleanup()
{
    for (auto ImageView : mImageViews) {
        vkDestroyImageView(FVulkanDynamicRHI::GetDevice(), ImageView, nullptr);
    }
    mImageViews.clear();
    mImages.clear();
    vkDestroySwapchainKHR(FVulkanDynamicRHI::GetDevice(), mSwapchain, nullptr);
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

VkPresentModeKHR FVulkanSwapchain::ChoosePresentMode(const std::vector<VkPresentModeKHR>& InAvailablePresentModes, VkPresentModeKHR InTargetPresentMode)
{
    for (const auto& AvailablePresentMode : InAvailablePresentModes) {
        if (AvailablePresentMode == InTargetPresentMode) {
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

void FVulkanSwapchain::CreateImageViews()
{
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

        REV_VK_CHECK_THROW(vkCreateImageView(FVulkanDynamicRHI::GetDevice(), &ImageViewCreateInfo, nullptr, &mImageViews[i]), "[FVkSwapChain] Failed to create image views!");
    }
}

}


