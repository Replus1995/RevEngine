#pragma once
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Core/Deleter.h"
#include <vector>
#include <vulkan/vulkan.h>

#include "Core/VulkanDefines.h"
#include "Core/VulkanInit.h"
#include "Core/VulkanDescriptor.h"
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
//Resource
	virtual void Bind(const Ref<FRHIVertexBuffer>& InVertexBuffer) override {};
	virtual void Bind(const Ref<FRHIIndexBuffer>& InIndexBuffer) override {};
	virtual void Bind(const Ref<FRHIVertexArray>& InVertexArray) override {};

	virtual void Bind(const Ref<FRHIShaderProgram>& InProgram) override {};
	virtual void Bind(const Ref<FRHITexture>& InTexture, uint32 InUnit) override {};
	virtual void Bind(const Ref<FRHIRenderTarget>& InRenderTarget) override {};

	virtual void Draw(const Ref<FRHIVertexArray>& InVertexArray) override {};


	const FVulkanSwapchain& GetSwapchain() const { return mSwapchain; }
	VkImage GetSwapchainImage() { return mSwapchain.GetImages()[mCurSwapchainImageIndex]; }
	VkImageView GetSwapchainImageView() { return mSwapchain.GetImageViews()[mCurSwapchainImageIndex]; }
	const FVulkanDescriptorPool& GetDescriptorPool() const { return mDescriptorPool; }

	FVulkanFrameData& GetFrameData() { return mFrameData[mFrameDataIndex]; }
	VkCommandBuffer GetMainCmdBuffer() { return mFrameData[mFrameDataIndex].MainCmdBuffer; }
private:
	void CreateImmediateData();

private:
	FVulkanSwapchain mSwapchain;
	FVulkanDescriptorPool mDescriptorPool;

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