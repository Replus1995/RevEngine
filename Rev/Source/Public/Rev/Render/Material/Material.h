#pragma once
#include "Rev/Math/Maths.h"
#include "Rev/Render/RenderCore.h"
#include <Rev/Render/RHI/RHIShader.h>
//#include <unordered_map>

namespace Rev
{

enum EMaterialDomain : uint8
{
	MD_Surface,
	MD_PostProcess
};

class FRHIShaderProgram;
class REV_API Material
{
public:
	Material();
	virtual ~Material();
	
	/**
	* @brief Create shader program based on ShadingModel
	*/
	virtual void Compile();
	virtual void PreDraw();
	virtual void PostDraw();
	const Ref<FRHIShaderProgram>& GetProgram() const
	{
		return mProgram;
	}

protected:
	Ref<FRHIShaderProgram> mProgram = nullptr;
	//EShadingModel mShadingModelCache = Rev::SM_Default;

public:
	EMaterialDomain Domain = MD_Surface;
	EBlendMode BlendMode = BM_Opaque;
	EShadingModel ShadingModel = SM_Default;
	bool TwoSided = false;
	float MaskClip = 0.5f;
};

}