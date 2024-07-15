#pragma once
#include "Rev/Math/Maths.h"
#include "Rev/Render/Material/MaterialUniform.h"
#include "Rev/Render/RenderCore.h"
#include <unordered_map>

namespace Rev
{

enum EMaterialDomain : uint8
{
	MD_Surface,
	MD_PostProcess
};

class MaterialUniform;
class REV_API Material
{
public:
	Material(const Ref<FRHIShaderProgram>& InProgram);
	virtual ~Material();
	
	const Ref<FRHIShaderProgram>& GetProgram() const
	{
		return mProgram;
	}

	void Bind();
	void Unbind();

	EMaterialDomain Domain = Rev::MD_Surface;
	EBlendMode BlendMode = Rev::BM_Opaque;
	EShadingModel ShadingModel = Rev::SM_Default;
	bool TwoSided = false;
	float MaskClip = 0.5f;

protected:
	virtual void UploadUniform();
protected:
	Ref<FRHIShaderProgram> mProgram = nullptr;
};

}