#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RHI/RHIResource.h"

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

enum ESamplerWarpMode
{
	SW_Repeat = 0,
	SW_Clamp,
	SW_Mirror
};

struct FSamplerDesc
{
	ESamplerFilterMode MinFilter = SF_Nearest;
	ESamplerFilterMode MagFilter = SF_Nearest;
	ESamplerWarpMode WarpU = SW_Repeat;
	ESamplerWarpMode WarpV = SW_Repeat;
	ESamplerWarpMode WarpW = SW_Repeat;
};

class FRHISamplerState : public FRHIResource
{
public:
	virtual ~FRHISamplerState() = default;
	const FSamplerDesc& GetDesc() const { return mDesc; }
protected:
	FRHISamplerState(const FSamplerDesc& InDesc) : mDesc(InDesc) {}
private:
	FSamplerDesc mDesc;
};

}