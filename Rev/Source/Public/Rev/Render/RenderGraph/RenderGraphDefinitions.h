#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RHI/RHIDefinitions.h"
#include "Rev/Render/RHI/RHITexture.h"
#include "Rev/Render/RHI/RHIBuffer.h"
#include <limits>

namespace Rev
{
enum class ERGPassFlags : uint8 { None = 0, Raster = 1 << 0, Copy = 1 << 1, NeverCull = 1 << 2 };
ENUM_CLASS_FLAGS(ERGPassFlags)

class REV_API FRGName
{
public:
	FRGName() = default;
	explicit FRGName(const char* InName) : Name(InName ? InName : "") {}
	explicit FRGName(std::string InName) : Name(std::move(InName)) {}
	const std::string& GetName() const { return Name; }
	const char* GetCharName() const { return Name.c_str(); }
private:
	std::string Name;
};

struct FRGTextureDesc : FRHITextureDesc
{
	FRGTextureDesc() = default;
	FRGTextureDesc(const FRHITextureDesc& InDesc) : FRHITextureDesc(InDesc) {}
	static FRGTextureDesc Create2D(uint16 Width, uint16 Height, EPixelFormat Format,
		const FRHITextureClearColor& Clear, ETextureCreateFlags Flags, uint8 NumMips = 1, uint8 NumSamples = 1)
	{
		return FRGTextureDesc(FRHITextureDesc::Create2D(Width, Height, Format).SetClearColor(Clear).SetFlags(Flags).SetNumMips(NumMips).SetNumSamples(NumSamples));
	}
};

struct FRGTextureHandle
{
	static constexpr uint32 InvalidIndex = std::numeric_limits<uint32>::max();
	uint32 ResourceIndex = InvalidIndex;
	uint32 Version = 0;
	bool IsValid() const { return ResourceIndex != InvalidIndex; }
	friend bool operator==(const FRGTextureHandle&, const FRGTextureHandle&) = default;
};
}
