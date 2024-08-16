#pragma once
#include "Rev/Render/RHI/RHIContext.h"
#include <vector>
#include <vulkan/vulkan.h>

#include "VkDefines.h"
#include "VkDevice.h"
#include "VkSwapChain.h"

namespace Rev
{

class FVkContext : public FRHIContext
{
public:
	virtual void Init() override;
	virtual void Cleanup() override;

//Command
	virtual void SetViewport(uint32 x, uint32 y, uint32 width, uint32 height) override {};
	virtual void SetClearColor(const Math::FLinearColor& color) override {};
	virtual void ClearBackBuffer() override {};

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

private:
	void CreateInstance();
	void CreateSurface();

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
	FVkSwapChain mSwapChain;
};

}