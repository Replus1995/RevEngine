#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RenderPipeline/PostPass.h"

namespace Rev
{
class Material;
class FGammaCorrectPass : public FPostPass
{
public:
	FGammaCorrectPass();
	virtual ~FGammaCorrectPass();
	virtual void BeginPass() override;
	virtual void RunPass() override;

protected:
	Ref<Material> mMaterial;

};


}
