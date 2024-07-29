#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RenderPipeline/PostPass.h"
#include "Rev/Render/RHI/RHIShader.h"

namespace Rev
{

class FGammaCorrectPass : public FPostPass
{
public:
	FGammaCorrectPass();
	virtual ~FGammaCorrectPass();
	void RunPass();

protected:
	Ref<FRHIShaderProgram> mProgram;

};


}
