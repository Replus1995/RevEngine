#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RHI/RHIDefinitions.h"
#include "Rev/Render/RHI/RHIResource.h"
#include "Rev/Render/RenderGraph/RenderGraphDefinitions.h"

namespace Rev
{

class REV_API FRGResource
{
public:
	FRGResource(const FRGResource&) = delete;
	virtual ~FRGResource() = default;

	// resource debug name
	const char* const Name = nullptr;

	FRHIResource* GetRHI() const { return ResourceRHI; }
protected:
	FRGResource(const char* InName) : Name(InName) {}
	FRHIResource* ResourceRHI = nullptr;

};

class REV_API FRGViewableResource : public FRGResource
{
public:
	virtual ~FRGViewableResource() = default;

	const ERGViewableResourceType Type;

	bool IsExternal() const { return bExternal; }
	bool IsExtracted() const { return bExtracted; }



protected:
	FRGViewableResource(const char* InName, ERGViewableResourceType InType)
		: FRGResource(InName)
		, Type(InType)
	{}

	/** Whether this is an externally registered resource. */
	bool bExternal;

	/** Whether this is an extracted resource. */
	bool bExtracted;

	/** Whether any sub-resource has been used for write by a pass. */
	bool bProduced;

	/** Whether this resource is allocated through the transient resource allocator. */
	bool bTransient;
};


class REV_API FRGTexture final : public FRGViewableResource
{
public:

	const FRGTextureDesc Desc;
	const ERGTextureFlags Flags;

	FORCEINLINE FRHITexture* GetRHI() const
	{
		return static_cast<FRHITexture*>(FRGResource::GetRHI());
	}

private:
	FRGTexture(const char* InName, const FRGTextureDesc& InDesc, ERGTextureFlags InFlags)
		: FRGViewableResource(InName, ERGViewableResourceType::Texture)
		, Desc(InDesc)
		, Flags(InFlags)
	{}

	friend FRGBuilder;
	friend FRGAllocator;
	friend FRGTextureRegistry;
};

class REV_API FRGBuffer final : public FRGViewableResource
{
public:
	
	const FRGBufferDesc Desc;
	const ERGBufferFlags Flags;

	FORCEINLINE FRHIBuffer* GetRHI() const
	{
		return static_cast<FRHIBuffer*>(FRGResource::GetRHI());
	}

private:
	FRGBuffer(const char* InName, const FRGBufferDesc& InDesc, ERGBufferFlags InFlags)
		: FRGViewableResource(InName, ERGViewableResourceType::Buffer)
		, Desc(InDesc)
		, Flags(InFlags)
	{}

	friend FRGBuilder;
	friend FRGAllocator;
	friend FRGBufferRegistry;
};

struct FRGColorTargetBinding
{
	FRGColorTargetBinding() = default;
	FRGColorTargetBinding(const FRGColorTargetBinding&) = default;

	FRGColorTargetBinding(FRGTexture* InTexture, ERenderTargetLoadAction InLoadAction, uint8 InMipIndex = 0, int16 InArraySlice = -1)
		: Texture(InTexture)
		, LoadAction(InLoadAction)
		, MipIndex(InMipIndex)
		, ArraySlice(InArraySlice)
	{
	}

	FRGColorTargetBinding(FRGTexture* InTexture, FRGTexture* InResolveTexture, ERenderTargetLoadAction InLoadAction, uint8 InMipIndex = 0, int16 InArraySlice = -1)
		: Texture(InTexture)
		, ResolveTexture(InResolveTexture)
		, LoadAction(InLoadAction)
		, MipIndex(InMipIndex)
		, ArraySlice(InArraySlice)
	{
	}

	bool IsValid() const
	{
		return Texture != nullptr;
	}

	bool NeedReslove() const
	{
		return Texture != nullptr && ResolveTexture != nullptr;
	}

	FRGTexture* Texture = nullptr;
	FRGTexture* ResolveTexture = nullptr;
	ERenderTargetLoadAction LoadAction = RTL_DontCare;
	uint8 MipIndex = 0;
	int16 ArraySlice = -1;
};

struct FRGDepthStencilTargetBinding
{
	FRGDepthStencilTargetBinding() = default;
	FRGDepthStencilTargetBinding(const FRGDepthStencilTargetBinding&) = default;

	FRGDepthStencilTargetBinding(FRGTexture* InTexture, ERenderTargetLoadAction InDepthLoadAction, ERenderTargetLoadAction InStencilLoadAction = RTL_DontCare)
		: Texture(InTexture)
		, DepthLoadAction(InDepthLoadAction)
		, StencilLoadAction(InStencilLoadAction)
	{
	}

	bool IsValid() const
	{
		return Texture != nullptr;
	}

	bool NeedReslove() const
	{
		return Texture != nullptr && ResolveTexture != nullptr;
	}

	FRGTexture* Texture = nullptr;
	FRGTexture* ResolveTexture = nullptr;
	ERenderTargetLoadAction DepthLoadAction = RTL_DontCare;
	ERenderTargetLoadAction StencilLoadAction = RTL_DontCare;
};

struct alignas(16) FRGRenderTargetBindings
{
	std::array<FRGColorTargetBinding, RTA_MaxColorAttachments> Colors;
	FRGDepthStencilTargetBinding DepthStencil;

	uint32 GetNumActive() const
	{
		uint32 Count = 0;
		for (; Colors[Count].IsValid(); ++Count) {}
		return Count;
	}

};

}
