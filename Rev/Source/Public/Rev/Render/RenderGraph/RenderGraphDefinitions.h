#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RHI/RHIDefinitions.h"
#include "Rev/Render/RHI/RHITexture.h"
#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Render/RenderGraph/RenderGraphAllocator.h"

namespace Rev
{

enum class ERGViewableResourceType : uint8
{
	Texture,
	Buffer,
	Count
};

enum class ERGTextureFlags : uint8
{
	None = 0,
	MultiFrame = 1 << 0,
};
ENUM_CLASS_FLAGS(ERGTextureFlags)

enum class ERGBufferFlags : uint8
{
	None = 0,
	MultiFrame = 1 << 0,
};
ENUM_CLASS_FLAGS(ERGBufferFlags)

enum class ERGPassFlags : uint8
{
	None = 0,
	Raster = 1 << 0,
	Compute = 1 << 1,
	Copy = 1 << 2,
};
ENUM_CLASS_FLAGS(ERGPassFlags)


struct FRGTextureDesc : public FRHITextureDesc
{
	static FRGTextureDesc Create2D(
		uint16 Width,
		uint16 Height,
		EPixelFormat Format,
		const FRHITextureClearColor& ClearColor,
		ETextureCreateFlags Flags,
		uint8 NumMips = 1,
		uint8 NumSamples = 1
	)
	{
		const uint16 Depth = 1;
		const uint16 ArraySize = 1;
		return FRGTextureDesc(ETextureDimension::Texture2D, Flags, Format, ClearColor, Width, Height, Depth, ArraySize, NumMips, NumSamples);
	}

	static FRGTextureDesc Create2DArray(
		uint16 Width,
		uint16 Height,
		EPixelFormat Format,
		const FRHITextureClearColor& ClearColor,
		ETextureCreateFlags Flags,
		uint16 ArraySize,
		uint8 NumMips = 1,
		uint8 NumSamples = 1
	)
	{
		const uint16 Depth = 1;
		return FRGTextureDesc(ETextureDimension::Texture2DArray, Flags, Format, ClearColor, Width, Height, Depth, ArraySize, NumMips, NumSamples);
	}

	static FRGTextureDesc Create3D(
		uint16 Width,
		uint16 Height,
		uint16 Depth,
		EPixelFormat Format,
		const FRHITextureClearColor& ClearColor,
		ETextureCreateFlags Flags,
		uint8 NumMips = 1
	)
	{
		const uint16 ArraySize = 1;
		const uint8 LocalNumSamples = 1;
		return FRGTextureDesc(ETextureDimension::Texture3D, Flags, Format, ClearColor, Width, Height, Depth, ArraySize, NumMips, LocalNumSamples);
	}

	static FRGTextureDesc CreateCube(
		uint32 Size,
		EPixelFormat Format,
		const FRHITextureClearColor& ClearColor,
		ETextureCreateFlags Flags,
		uint8 NumMips = 1,
		uint8 NumSamples = 1
	)
	{
		const uint16 Depth = 1;
		const uint16 ArraySize = 1;
		return FRGTextureDesc(ETextureDimension::TextureCube, Flags, Format, ClearColor, Size, Size, Depth, ArraySize, NumMips, NumSamples);
	}

	static FRGTextureDesc CreateCubeArray(
		uint32 Size,
		EPixelFormat Format,
		const FRHITextureClearColor& ClearColor,
		ETextureCreateFlags Flags,
		uint16 ArraySize,
		uint8 NumMips = 1,
		uint8 NumSamples = 1
	)
	{
		const uint16 Depth = 1;
		return FRGTextureDesc(ETextureDimension::TextureCubeArray, Flags, Format, ClearColor, Size, Size, Depth, ArraySize, NumMips, NumSamples);
	}

	FRGTextureDesc() = default;
	FRGTextureDesc(
		ETextureDimension InDimension,
		ETextureCreateFlags InFlags,
		EPixelFormat InFormat,
		const FRHITextureClearColor& InClearValue,
		uint16 InWidth,
		uint16 InHeight,
		uint16 InDepth,
		uint16 InArraySize,
		uint8 InNumMips,
		uint8 InNumSamples
	)
		: FRHITextureDesc(InDimension, InFlags, InFormat, InClearValue, InWidth, InHeight, InDepth, InArraySize, InNumMips, InNumSamples)
	{
	}

};

struct FRGBufferDesc
{
	static FRGBufferDesc CreateBuffer(uint32 ElementSize, uint32 NumElements)
	{
		FRGBufferDesc Desc;
		Desc.Usage = EBufferUsageFlags::Static | EBufferUsageFlags::ShaderResource | EBufferUsageFlags::VertexBuffer;
		Desc.ElementSize = ElementSize;
		Desc.NumElements = NumElements;
		return Desc;
	}

	uint32 GetSize() const
	{
		return ElementSize * NumElements;
	}

	bool operator == (const FRGBufferDesc& Other) const
	{
		bool bSame = ElementSize == Other.ElementSize &&
			NumElements == Other.NumElements &&
			Usage == Other.Usage;
		return bSame;
	}

	bool operator != (const FRGBufferDesc& Other) const
	{
		return !(*this == Other);
	}


	uint32 ElementSize = 1;
	uint32 NumElements = 1;
	EBufferUsageFlags Usage = EBufferUsageFlags::None;
};

class REV_API FRGName final
{
public:
	FRGName() = default;
	explicit FRGName(const char* InName) : Name(InName) {}
	FRGName(const FRGName& Other) : Name(Other.Name) {}
	FRGName(FRGName&& Other) noexcept : Name(std::move(Other.Name)) {}

	const std::string& GetName() const { return Name; }
	const char* GetCharName() const { return Name.c_str(); }
private:
	std::string Name;
};

class FRGResource;
using FRGResourceRef = FRGResource*;

class FRGTexture;
using FRGTextureRef = FRGTexture*;

class FRGBuffer;
using FRGBufferRef = FRGBuffer*;

class FRGUniformBuffer;
using FRGUniformBufferRef = FRGUniformBuffer*;

class FRGPass;
using FRGPassRef = FRGPass*;

class FRGBuilder;
class FRGParameterStruct;

struct FRGColorTargetBinding;
struct FRGDepthStencilTargetBinding;
struct FRGRenderTargetBindings;

template <typename InObjectType, typename InIndexType>
class TRGHandle
{
public:
	using ObjectType = InObjectType;
	using IndexType = InIndexType;

	TRGHandle() = default;
	explicit TRGHandle(int32 InIndex)
	{
		REV_CORE_ASSERT(InIndex >= 0 && InIndex <= kNullIndex)
		Index = (IndexType)InIndex;
	}
	TRGHandle(TRGHandle&&) = default;

	IndexType GetIndex() const { return Index; }
	bool IsNull() const { return Index == kNullIndex; }
	bool IsValid() const { return Index != kNullIndex; }

private:
	static const IndexType kNullIndex = std::numeric_limits<InIndexType>::max();
	IndexType Index = kNullIndex;
};

enum class ERGHandleDestructPolicy
{
	Registry,
	Allocator,
	Never,
};


template<typename InHandleType, ERGHandleDestructPolicy DestructPolicy = ERGHandleDestructPolicy::Registry>
class TRGHandleRegistry
{
public:
	using HandleType = InHandleType;
	using ObjectType = typename HandleType::ObjectType;
	using IndexType = typename HandleType::IndexType;

	TRGHandleRegistry() = default;
	TRGHandleRegistry(const TRGHandleRegistry&) = delete;
	~TRGHandleRegistry()
	{

	}

	FORCEINLINE HandleType Begin()
	{
		return HandleType(0);
	}

	FORCEINLINE HandleType End()
	{
		return HandleType(ObjectArray.size());
	}

	FORCEINLINE HandleType Last()
	{
		return HandleType(ObjectArray.size() - 1);
	}

	void Add(ObjectType* Object)
	{
		ObjectArray.emplace_back(Object);
		Object->Handle = Last();
	}

	void Clear()
	{
		if (DestructPolicy == ERGHandleDestructPolicy::Registry)
		{
			for (uint32 index = ObjectArray.size() - 1; index >= 0; index--)
			{
				ObjectArray[index]->~ObjectType();
			}
		}
		ObjectArray.clear();
	}

	template<typename DerivedType = ObjectType, typename... TArgs>
	DerivedType* Allocate(FRGAllocator& Allocator, TArgs&&... Args)
	{
		static_assert(std::derived_from<DerivedType, ObjectType>::value, "Registy try to allocate an invalid type");
		DerivedType* Object;
		if (DestructPolicy == ERGHandleDestructPolicy::Allocator)
		{
			Object = Allocator.Alloc<DerivedType>(std::forward<TArgs&&>(Args)...);
		}
		else
		{
			Object = Allocator.AllocNoDestruct<DerivedType>(std::forward<TArgs&&>(Args)...);
		}
		Add(Object);
		return Object;
	}

	template<typename FunctionType>
	void ForEach(FunctionType Function)
	{
		for (ObjectType* Object : ObjectArray)
		{
			Function(Object);
		}
	}

	template<typename FunctionType>
	void ForEach(FunctionType Function) const
	{
		for (const ObjectType* Object : ObjectArray)
		{
			Function(Object);
		}
	}

private:
	std::vector<ObjectType*> ObjectArray;
};

using FRGTextureHandle = TRGHandle<FRGTexture, uint16>;
using FRGTextureRegistry = TRGHandleRegistry<FRGTextureHandle>;

using FRGBufferHandle = TRGHandle<FRGBuffer, uint16>;
using FRGBufferRegistry = TRGHandleRegistry<FRGBufferHandle>;

using FRGPassHandle = TRGHandle<FRGPass, uint16>;
using FRGPassRegistry = TRGHandleRegistry<FRGPassHandle>;

}