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
	virtual void SyncUniform();
	const Ref<FRHIShaderProgram>& GetProgram() const
	{
		return mProgram;
	}

protected:
	Ref<FRHIShaderProgram> mProgram = nullptr;
	//EShadingModel mShadingModelCache = Rev::SM_Default;

public:
	EMaterialDomain TessEval = Rev::MD_Surface;
	EBlendMode BlendMode = Rev::BM_Opaque;
	EShadingModel ShadingModel = Rev::SM_Default;
	bool TwoSided = false;
	float MaskClip = 0.5f;
};

}