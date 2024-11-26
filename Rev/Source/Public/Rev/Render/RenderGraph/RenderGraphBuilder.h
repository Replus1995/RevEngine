#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RHI/RHIDefinitions.h"
#include "Rev/Render/RenderGraph/RenderGraphDefinitions.h"
#include "Rev/Render/RenderGraph/RenderGraphAllocator.h"
#include "Rev/Render/RenderGraph/RenderGraphResources.h"
#include "Rev/Render/RenderGraph/RenderGraphPass.h"

namespace Rev
{

class FRHITexture;
class FRHIBuffer;

class REV_API FRGBuilder : public FRGAllocatorScope
{
public:
	FRGBuilder(const FRGName& InName = {});
	FRGBuilder(const FRGBuilder&) = delete;
	virtual ~FRGBuilder();


	//Resource Functions
	FRGTextureRef RegisterExternalTexture(const Ref<FRHITexture>& ExternalTexture, ERGTextureFlags InFlags = ERGTextureFlags::None);

	FRGBufferRef RegisterExternalBuffer(const Ref<FRHIBuffer>& ExternalBuffer, ERGBufferFlags InFlags = ERGBufferFlags::None);

	FRGTextureRef CreateTexture(const FRGTextureDesc& InDesc, const char* InName, ERGTextureFlags InFlags = ERGTextureFlags::None);

	FRGBufferRef CreateBuffer(const FRGBufferDesc& InDesc, const char* InName, ERGBufferFlags InFlags = ERGBufferFlags::None);

	//Alloc Functions
	void* Alloc(uint64 SizeInBytes, uint32 AlignInBytes = 16);

	template <typename PODType>
	PODType* AllocPOD();

	template <typename PODType>
	PODType* AllocPODArray(uint32 Count);

	template <typename ObjectType, typename... TArgs>
	ObjectType* AllocObject(TArgs&&... Args);

	template <typename ParameterStructType>
	ParameterStructType* AllocParameters();

	template <typename ParameterStructType>
	ParameterStructType* AllocParameters(ParameterStructType* StructToCopy);

	//Pass Functions
	template<typename ParameterStructType>
	using PrepareLambdaFunc = std::function<void(FRGPassRef, const ParameterStructType*)>;

	template<typename ParameterStructType>
	using ExecuteLambdaFunc = std::function<void(FRHICommandList&, const ParameterStructType*)>;

	template <typename ParameterStructType>
	FRGPassRef AddPass(FRGName&& InName, const ParameterStructType* ParameterStruct, ERGPassFlags Flags, PrepareLambdaFunc<ParameterStructType>&& PrepareLambda, ExecuteLambdaFunc<ParameterStructType>&& ExecuteLambda);

private:
	const FRGName BuilderName;
	FRGTextureRegistry TextureRegistry;
	FRGBufferRegistry BufferRegistry;
	FRGPassRegistry PassRegistry;

	//Transient Resource Allocator
};


}

#include "RenderGraphBuilder.inl"