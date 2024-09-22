#pragma once
#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Render/RHI/RHISampler.h"
#include "Rev/Render/RHI/RHITexture.h"
#include "Rev/Render/RHI/RHIRenderTarget.h"

namespace Rev
{

class REV_API FRHIResourceFactory
{
public:
	//Resource
	static Ref<FRHIVertexBuffer> CreateVertexBuffer(uint32 InSize); //Dynamic Data
	static Ref<FRHIVertexBuffer> CreateVertexBuffer(const float* InVertices, uint32 InSize); //Static Data
	static Ref<FRHIIndexBuffer> CreateIndexBuffer(uint32 InStride, uint32 InCount); //Dynamic Data
	static Ref<FRHIIndexBuffer> CreateIndexBuffer(const void* InIndices, uint32 InStride, uint32 InCount); //Static Data
	static Ref<FRHIVertexArray> CreateVertexArray();
	static Ref<FRHIUniformBuffer> CreateUniformBuffer(uint32 InSize);

	//static Ref<FRHISampler> CreateSampler(const FSamplerDesc& InDesc);
	static Ref<FRHITexture> CreateTexture(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc);
	static Ref<FRHIRenderTarget> CreateRenderTarget(const FRenderTargetDesc& InDesc);

	//State

};

}