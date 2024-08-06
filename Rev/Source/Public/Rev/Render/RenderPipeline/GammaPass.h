#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RenderPipeline/FullScreenPass.h"

namespace Rev
{
class Material;
class FGammaCorrectPass : public FFullScreenPass
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
