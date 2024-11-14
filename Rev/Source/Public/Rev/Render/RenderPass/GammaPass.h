#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RenderPass/RenderPass.h"

namespace Rev
{
class FMaterial;
class FGammaCorrectPass : public FRenderPass
{
public:
	FGammaCorrectPass();
	virtual ~FGammaCorrectPass();
	virtual void BeginPass() override;
	virtual void RunPass() override;

protected:
	Ref<FMaterial> mMaterial;

};


}
