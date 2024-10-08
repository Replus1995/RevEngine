#pragma once
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Core/Deleter.h"
#include <vector>
#include <vulkan/vulkan.h>

#include "Core/VulkanDefines.h"
#include "Core/VulkanInit.h"
#include "VulkanSwapchain.h"
#include "VulkanFrameData.h"

namespace Rev
{

class FVulkanContext : public FRHIContext
{
public:
	FVulkanContext();
	virtual ~FVulkanContext();

	virtual void Init() override;
	virtual void Cleanup() override;
	virtual void Flush() override;

	virtual void BeginFrame(bool bClearBackBuffer) override;
	virtual void EndFrame() override;
	virtual void PresentFrame() override;

	virtual void ImmediateSubmit(std::function<void(VkCommandBuffer)>&& Func);

//Command
	virtual void SetViewport(uint32 InX, uint32 InY, uint32 InWidth, uint32 InHeight) override;
	virtual void SetClearColor(const Math::FLinearColor& InColor) override;
	virtual void SetClearDepthStencil(float InDepth, uint32 InStencil);
	virtual void ClearBackBuffer() override;

	virtual void DrawPrimitive(const Ref<FRHIPrimitive>& InPrimitive, const Ref<FRHIShaderProgram>& InProgram) override;


	const FVulkanSwapchain& GetSwapchain() const { return mSwapchain; }
	VkImage GetSwapchainImage() { return mSwapchain.GetImages()[mCurSwapchainImageIndex]; }
	VkImageView GetSwapchainImageView() { return mSwapchain.GetImageViews()[mCurSwapchainImageIndex]; }

	FVulkanFrameData& GetFrameData() { return mFrameData[mFrameDataIndex]; }
	VkCommandBuffer GetMainCmdBuffer() { return mFrameData[mFrameDataIndex].MainCmdBuffer; }

	FVulkanDescriptorPool& GetDescriptorPool() { return mFrameData[mFrameDataIndex].DescriptorPool; }

private:
	void CreateImmediateData();

private:
	FVulkanSwapchain mSwapchain;

	//frame data
	uint32 mCurSwapchainImageIndex = 0;
	uint32 mFrameDataIndex = 0;
	FVulkanFrameData mFrameData[REV_VK_FRAME_OVERLAP];

	//
	VkClearColorValue mClearColor;
	VkClearDepthStencilValue mClearDepthStencil;
	VkExtent2D mDrawExtent = {0, 0};

	//immediate draw
	VkFence mImmFence;
	VkCommandBuffer mImmCmdBuffer;
	VkCommandPool mImmCmdPool;
};


}