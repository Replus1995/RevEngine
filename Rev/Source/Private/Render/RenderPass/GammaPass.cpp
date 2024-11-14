#include "Rev/Render/RenderPass/GammaPass.h"
#include "Rev/Render/RHI/RHIShaderLibrary.h"
#include "Rev/Render/RenderUtils.h"
#include "Rev/Render/Material/Material.h"

namespace Rev
{

class GammaCorrectMaterial : public FMaterial
{
public:
	GammaCorrectMaterial()
	{
		Domain = MD_PostProcess;
	}
	virtual ~GammaCorrectMaterial() = default;

	virtual void Compile() override
	{
		ShaderProgram = FRHIShaderLibrary::Get()->CreateGraphicsProgram(
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
	/*mMaterial->PreDraw();
	RenderUtils::DrawScreenQuad();
	mMaterial->PostDraw();*/
}


}

