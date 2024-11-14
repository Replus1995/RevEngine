#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Render/RHI/RHIState.h"
#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Render/RHI/RHITexture.h"
#include "Rev/Render/RHI/RHIRenderPass.h"

namespace Rev
{

class REV_API IDyanmicRHI
{
public:
	IDyanmicRHI() = default;
	virtual ~IDyanmicRHI() = default;

	virtual void Init() = 0;
	virtual void Cleanup() = 0;
	virtual const FRHIDeviceCapacity& GetDeviceCapacity() = 0;

	virtual Scope<IRHIContext> RHICreateContext() = 0;
	//Buffer
	virtual Ref<FRHIBuffer> RHICreateBuffer(const FRHIBufferDesc& InDesc) = 0;
	virtual Ref<FRHIUniformBuffer> RHICreateUniformBuffer(uint32 InSize) = 0;

	//Texture
	virtual Ref<FRHITexture> RHICreateTexture(const FRHITextureDesc& InDesc) = 0;
	virtual void RHIResizeTexture(FRHITexture* InTexture, uint32 InWidth, uint32 InHeight, uint32 InDepth) = 0;

	//State
	virtual Ref<FRHISamplerState> RHICreateSamplerState(const FRHISamplerStateDesc& InDesc) = 0;
	virtual Ref<FRHIRasterizerState> RHICreateRasterizerState(const FRHIRasterizerStateDesc& InDesc) = 0;
	virtual Ref<FRHIDepthStencilState> RHICreateDepthStencilStateState(const FRHIDepthStencilStateDesc& InDesc) = 0;
	virtual Ref<FRHIColorBlendState> RHICreateColorBlendState(const FRHIColorBlendStateDesc& InDesc) = 0;
	virtual Ref<FRHIVertexInputState> RHICreateVertexInputState(const FRHIVertexInputStateDesc& InDesc) = 0;

	//Pipeline
	virtual Ref<FRHIRenderPass> RHICreateRenderPass(const FRHIRenderPassDesc& InDesc) = 0;

};

extern REV_API IDyanmicRHI* GDynamicRHI;

void RHIInit(ERenderAPI InAPI);
void RHIExit();

}