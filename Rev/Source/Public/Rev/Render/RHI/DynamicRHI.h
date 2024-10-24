#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Render/RHI/RHIState.h"
#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Render/RHI/RHITexture.h"
#include "Rev/Render/RHI/RHIRenderTarget.h"
#include "Rev/Render/RHI/RHIPrimitive.h"
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

	virtual Scope<FRHIContext> RHICreateContext() = 0;
	//Buffer
	virtual Ref<FRHIVertexBuffer> CreateVertexBuffer(uint32 InSize, bool bDynamic = false) = 0;
	virtual Ref<FRHIIndexBuffer> CreateIndexBuffer(uint32 InStride, uint32 InCount, bool bDynamic = false) = 0;
	virtual Ref<FRHIUniformBuffer> CreateUniformBuffer(uint32 InSize) = 0;
	virtual Ref<FRHIBuffer> RHICreateBuffer(uint32 InSize, uint32 InStride, EBufferUsageFlags InFlags) = 0;

	//Primitive
	virtual Ref<FRHIPrimitive> CreatePrimitive(EPrimitiveTopology InTopology) = 0;

	//Texture
	virtual Ref<FRHITexture> CreateTexture(const FRHITextureDesc& InDesc) = 0;
	virtual Ref<FRHIRenderTarget> CreateRenderTarget(const FRenderTargetDesc& InDesc) = 0;

	//State
	virtual Ref<FRHISamplerState> RHICreateSamplerState(const FRHISamplerStateDesc& InDesc) = 0;
	virtual Ref<FRHIRasterizerState> RHICreateRasterizerState(const FRHIRasterizerStateDesc& InDesc) = 0;
	virtual Ref<FRHIDepthStencilState> RHICreateDepthStencilStateState(const FRHIDepthStencilStateDesc& InDesc) = 0;
	virtual Ref<FRHIColorBlendState> RHICreateColorBlendState(const FRHIColorBlendStateDesc& InDesc) = 0;
	virtual Ref<FRHIVertexInputState> RHICreateVertexInputState(const FRHIVertexInputStateDesc& InDesc) = 0;

	//Pipeline
	virtual Ref<FRHIRenderPass> CreateRenderPass(const FRHIRenderPassDesc& InDesc) = 0;

};

extern REV_API IDyanmicRHI* GDynamicRHI;

void RHIInit(ERenderAPI InAPI);
void RHIExit();

}