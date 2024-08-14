#pragma once
#include "Rev/Render/RHI/RHIContext.h"

#include <vulkan/vulkan.h>

namespace Rev
{

class FVkContext : public FRHIContext
{
public:
	virtual void Init() override;
	virtual void Cleanup() override;

//Command
	virtual void SetViewport(uint32 x, uint32 y, uint32 width, uint32 height) override;
	virtual void SetClearColor(const Math::FLinearColor& color) override;
	virtual void ClearBackBuffer() override;

	virtual void EnableDepthTest(bool bEnable) override;
	virtual void EnableDepthWrite(bool bEnable) override;
	virtual void SetDepthTestMode(EDepthTestMode InMode) override;
	virtual void SetBlendMode(EBlendMode InMode) override;
	virtual void SetCullFaceMode(ECullFaceMode InMode) override;

//Resource
	virtual void Bind(const Ref<FRHIVertexBuffer>& InVertexBuffer) override;
	virtual void Bind(const Ref<FRHIIndexBuffer>& InIndexBuffer) override;
	virtual void Bind(const Ref<FRHIVertexArray>& InVertexArray) override;

	virtual void Bind(const Ref<FRHIShaderProgram>& InProgram) override;
	virtual void Bind(const Ref<FRHIUniformBuffer>& InUniformBuffer, uint32 InUnit) override;
	virtual void Bind(const Ref<FRHITexture>& InTexture, uint32 InUnit) override;
	virtual void Bind(const Ref<FRHIRenderTarget>& InRenderTarget) override;

	virtual void Draw(const Ref<FRHIVertexArray>& InVertexArray, EDrawMode InDrawMode) override;

private:
	void QueryExtensionSupport();
	std::vector<const char*> CheckValidationLayerSupport(const std::vector<const char*>& InValidationLayers);

private:
	VkInstance mInstance;
};

}