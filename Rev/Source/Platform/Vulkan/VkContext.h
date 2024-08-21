#pragma once
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Core/Deleter.h"
#include <vector>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "VkDefines.h"
#include "VkDevice.h"
#include "VkSwapchain.h"
#include "VkFrameData.h"

namespace Rev
{

class FVkContext : public FRHIContext
{
public:
	FVkContext();
	virtual ~FVkContext();

	virtual void Init() override;
	virtual void Cleanup() override;

	virtual void BeginFrame() override;
	virtual void EndFrame() override;

//Command
	virtual void SetViewport(uint32 x, uint32 y, uint32 width, uint32 height) override {};
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


	const VkInstance& GetInstance() const { return mInstance; }
	const VkSurfaceKHR& GetSurface() const { return mSurface; }
	const VmaAllocator& GetAllocator() const { return mAllocator; }

	FVkFrameData& GetFrameData() { return mFrameData[mFrameDataIndex]; }
	VkCommandBuffer GetMainCmdBuffer() { return mFrameData[mFrameDataIndex].MainCmdBuffer; }
	
private:
	void CreateInstance();
	void CreateSurface();
	void CreateAllocator();

private:
	static void CheckExtensionSupport(const std::vector<const char*>& InExtensionNames);
	static void CheckLayerSupport(const std::vector<const char*>& InLayerNames);
	static std::vector<const char*> GetEnabledExtensions();
	static std::vector<const char*> GetEnabledLayers();

private:
	VkInstance mInstance = VK_NULL_HANDLE;
	//VkDebugUtilsMessengerEXT mDebugMessenger = VK_NULL_HANDLE;
	VkSurfaceKHR mSurface = VK_NULL_HANDLE;
	FVkDevice mDevice;
	FVkSwapchain mSwapchain;
	VmaAllocator mAllocator = nullptr;

	FDeletorQueue mMainDeletorQueue;

	//frame data
	uint32 mCurSwapchainImageIndex = 0;
	uint32 mFrameDataIndex = 0;
	FVkFrameData mFrameData[REV_VK_FRAME_OVERLAP];

	VkClearColorValue mClearColor;
	VkClearDepthStencilValue mClearDepthStencil;

	VkExtent2D mDrawExtent = {0, 0};

};

}