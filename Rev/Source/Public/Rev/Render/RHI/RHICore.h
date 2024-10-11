#pragma once
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Render/RHI/RHISampler.h"
#include "Rev/Render/RHI/RHITexture.h"
#include "Rev/Render/RHI/RHIRenderTarget.h"
#include "Rev/Render/RHI/RHIPrimitive.h"
#include "Rev/Render/RHI/RHIRenderPass.h"

namespace Rev
{

class REV_API FRHICore
{
public:
	static void Init();
	static void Cleanup();

	static Scope<FRHIContext> CreateContext();
	//Buffer
	static Ref<FRHIVertexBuffer> CreateVertexBuffer(uint32 InSize); //Dynamic Data
	static Ref<FRHIVertexBuffer> CreateVertexBuffer(const float* InVertices, uint32 InSize); //Static Data
	static Ref<FRHIIndexBuffer> CreateIndexBuffer(EIndexElementType InType, uint32 InCount); //Dynamic Data
	static Ref<FRHIIndexBuffer> CreateIndexBuffer(const void* InIndices, EIndexElementType InType, uint32 InCount); //Static Data

	static Ref<FRHIUniformBuffer> CreateUniformBuffer(uint32 InSize);
	static Ref<FRHIUniformBufferDynamic> CreateUniformBufferDynamic(uint32 InSize);

	//Primitive
	static Ref<FRHIPrimitive> CreatePrimitive(EPrimitiveTopology InTopology);

	//Texture
	static Ref<FRHITexture> CreateTexture(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc);
	static Ref<FRHIRenderTarget> CreateRenderTarget(const FRenderTargetDesc& InDesc);

	//Pipeline
	static Ref<FRHIRenderPass> CreateRenderPass(const FRenderPassDesc& InDesc);

};

}