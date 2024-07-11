#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RHI/RHIResource.h"
#include "Rev/Math/Maths.h"

namespace Rev
{

enum ESamplerFilterMode
{
	SF_Nearest = 0,
	SF_Bilinear,
	SF_Trilinear,
	SF_AnisotropicNearest,
	SF_AnisotropicLinear,
};

enum ESamplerAnisotropicMode
{
	SA_NONE = 1,
	SA_2X = 2,
	SA_4X = 4,
	SA_8X = 8,
	SA_16X = 16
};

enum ESamplerWarpMode
{
	SW_Repeat = 0,
	SW_Clamp,
	SW_Mirror,
	SW_Border
};

struct FSamplerDesc
{
	ESamplerFilterMode Filter = SF_Nearest;
	ESamplerAnisotropicMode Anisotropic = SA_NONE;
	ESamplerWarpMode WarpU = SW_Repeat;
	ESamplerWarpMode WarpV = SW_Repeat;
	ESamplerWarpMode WarpW = SW_Repeat;
	Math::FLinearColor BorderColor = Math::FLinearColor(0,0,0,1);
};

class FRHISampler : public FRHIResource
{
public:
	virtual ~FRHISampler() = default;
	const FSamplerDesc& GetDesc() const { return mDesc; }
protected:
	FRHISampler(const FSamplerDesc& InDesc) : mDesc(InDesc) {}
protected:
	FSamplerDesc mDesc;
};

}