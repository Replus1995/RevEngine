#pragma once
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Core/Deleter.h"
#include <vector>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "Core/VkDefines.h"
#include "Core/VkInstance.h"
#include "Core/VkDevice.h"
#include "Core/VkSwapchain.h"
#include "Core/VkFrameData.h"
#include "VkInitializer.h"

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

	virtual void EnableDepthTest(bool bEnable) override {};
	virtual void EnableDepthWrite(bool bEnable) override {};
	virtual void SetDepthTestMode(EDepthTestMode InMode) override {};
	virtual void SetBlendMode(EBlendMode InMode) override {};
	virtual void SetCullFaceMode(ECullFaceMode InMode) override {};

//Resource
	virtual void Bind(const Ref<FRHIVertexBuffer>& InVertexBuffer) override {};
	virtual void Bind(const Ref<FRHIIndexBuffer>& InIndexBuffer) override {};
	virtual void Bind(const Ref<FRHIVertexArray>& InVertexArray) override {};

	virtual void Bind(const Ref<FRHIShaderProgram>& InProgram) override {};
	virtual void Bind(const Ref<FRHIUniformBuffer>& InUniformBuffer, uint32 InUnit) override {};
	virtual void Bind(const Ref<FRHITexture>& InTexture, uint32 InUnit) override {};
	virtual void Bind(const Ref<FRHIRenderTarget>& InRenderTarget) override {};

	virtual void Draw(const Ref<FRHIVertexArray>& InVertexArray, EDrawMode InDrawMode) override {};


	const FVulkanInstance& GetInstance() const { return mInstance; }
	const FVulkanDevice& GetDevice() const { return mDevice; }
	const FVulkanSwapchain& GetSwapchain() const { return mSwapchain; }
	const VmaAllocator& GetAllocator() const { return mAllocator; }


	FVulkanFrameData& GetFrameData() { return mFrameData[mFrameDataIndex]; }
	VkCommandBuffer GetMainCmdBuffer() { return mFrameData[mFrameDataIndex].MainCmdBuffer; }
	VkImage GetSwapchainImage() { return mSwapchain.GetImages()[mCurSwapchainImageIndex]; }
	VkImageView GetSwapchainImageView() { return mSwapchain.GetImageViews()[mCurSwapchainImageIndex]; }
	
private:
	void CreateAllocator();
	void CreateImmediateData();

private:
	FVulkanInstance mInstance;
	FVulkanDevice mDevice;
	FVulkanSwapchain mSwapchain;
	VmaAllocator mAllocator = nullptr;

	FDeletorQueue mMainDeletorQueue;

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