#pragma once
#include "Rev/Render/RHI/RHIDefinitions.h"
#include "Rev/Render/RHI/RHIResource.h"
#include "Rev/Math/Maths.h"

namespace Rev
{

struct FSamplerDesc
{
	ESamplerFilterMode Filter = SF_Nearest;
	ESamplerWarpMode WarpU = SW_Repeat;
	ESamplerWarpMode WarpV = SW_Repeat;
	ESamplerWarpMode WarpW = SW_Repeat;
	ESamplerAnisotropicMode Anisotropic = SA_None;
	Math::FLinearColor BorderColor = Math::FLinearColor(0,0,0,1); //Move to other place
};

//inline bool operator==(const FSamplerDesc& InA, const FSamplerDesc& InB)
//{
//	return InA.Filter == InB.Filter &&
//	InA.Anisotropic == InB.Anisotropic &&
//	InA.WarpU == InB.WarpU &&
//	InA.WarpV == InB.WarpV &&
//	InA.WarpW == InB.WarpW;
//}
//
//inline bool operator!=(const FSamplerDesc& InA, const FSamplerDesc& InB)
//{
//	return InA.Filter != InB.Filter ||
//		InA.Anisotropic != InB.Anisotropic ||
//		InA.WarpU != InB.WarpU ||
//		InA.WarpV != InB.WarpV ||
//		InA.WarpW != InB.WarpW;
//}

//TODO: Add FSamplerDesc hash function

class FRHISampler : public FRHIResource
{
public:
	FRHISampler(const FSamplerDesc& InDesc) : mDesc(InDesc) {}
	virtual ~FRHISampler() = default;

	const FSamplerDesc& GetDesc() const { return mDesc; }

	/*virtual void SetFilterMode(ESamplerFilterMode InMode) = 0;
	virtual void SetAnisotropicMode(ESamplerAnisotropicMode InMode) = 0;
	virtual void SetWarpU(ESamplerWarpMode InMode) = 0;
	virtual void SetWarpV(ESamplerWarpMode InMode) = 0;
	virtual void SetWarpW(ESamplerWarpMode InMode) = 0;
	virtual void SetBorderColor(const Math::FLinearColor& InColor) = 0;*/

protected:
	FSamplerDesc mDesc;
};

}