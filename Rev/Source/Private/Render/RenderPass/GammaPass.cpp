#include "Rev/Render/RenderPass/GammaPass.h"
#include "Rev/Render/RHI/RHIResourceFactory.h"
#include "Rev/Render/RHI/RHIShaderLibrary.h"
#include "Rev/Render/RenderCmd.h"
#include "Rev/Render/RenderUtils.h"
#include "Rev/Render/Material/Material.h"

namespace Rev
{

class GammaCorrectMaterial : public Material
{
public:
	GammaCorrectMaterial()
	{
		mDomain = MD_PostProcess;
	}
	virtual ~GammaCorrectMaterial() = default;

	virtual void Compile() override
	{
		mProgram = FRHIShaderLibrary::GetInstance().CreateGraphicsProgram(
			"GammaCorrectProgram",
			{ "/Engine/Shaders/PostVS" },
			{ "/Engine/Shaders/GammaCorrectFS" }
		);
	}
};


FGammaCorrectPass::FGammaCorrectPass()
	: FRenderPass("GammaCorrectPass")
{
}

FGammaCorrectPass::~FGammaCorrectPass()
{
}

void FGammaCorrectPass::BeginPass()
{
	FRenderPass::BeginPass();
	if (!mMaterial)
	{
		mMaterial = CreateRef<GammaCorrectMaterial>();
		mMaterial->Compile();
	}
}

void FGammaCorrectPass::RunPass()
{
	ClearRenderTarget();
	mMaterial->PreDraw();
	RenderUtils::DrawScreenQuad();
	mMaterial->PostDraw();
}


}

