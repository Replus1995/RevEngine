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
class REV_API Material
{
public:
	Material();
	virtual ~Material();
	
	/**
	* @brief Create shader program based on ShadingModel
	*/
	virtual void Compile() = 0;
	virtual void PreDraw(FRHICommandList& RHICmdList); //Bind Program
	virtual void PostDraw(FRHICommandList& RHICmdList); //Unbind Program
	//const Ref<FRHIShaderProgram>& GetProgram() const { return mProgram; }

	EMaterialDomain GetDomain() const { return mDomain; }

	const Ref<FRHIShaderProgram>& GetProgram() const { return mProgram; }

protected:
	Ref<FRHIShaderProgram> mProgram = nullptr;
	EMaterialDomain mDomain = MD_Surface;
};

}