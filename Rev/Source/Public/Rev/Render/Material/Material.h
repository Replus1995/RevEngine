#pragma once
#include "Rev/Core/Base.h"

namespace Rev
{

enum EMaterialDomain : uint8
{
	MD_Surface,
	MD_PostProcess
};

enum EMaterialBlendMode : uint8
{
	MBM_Diabled = 0,
	MBM_Opaque = 1,
	MBM_Transparent,
	MBM_Masked
};

enum EMaterialShadingModel : uint8
{
	MSM_Default = 0,
	MSM_Unlit
};

class FRHICommandList;
class FRHIShaderProgram;
class REV_API FMaterial
{
public:
	FMaterial();
	virtual ~FMaterial();
	
	/**
	* @brief Create shader program based on ShadingModel
	*/
	virtual void Compile() = 0;
	virtual void PreDraw(FRHICommandList& RHICmdList); //Bind Program
	virtual void PostDraw(FRHICommandList& RHICmdList); //Unbind Program
	const Ref<FRHIShaderProgram>& GetProgram() const { return ShaderProgram; }

	EMaterialDomain Domain = MD_Surface;
	EMaterialBlendMode BlendMode = MBM_Opaque;
	EMaterialShadingModel ShadingModel = MSM_Default;
	bool TwoSided = false;
	float MaskClip = 0.5f;

protected:
	Ref<FRHIShaderProgram> ShaderProgram = nullptr;
};

}