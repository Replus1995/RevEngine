#include "Rev/Render/RenderPipeline/SkyboxPass.h"
#include "Rev/Render/Material/Material.h"
#include "Rev/Render/RenderCmd.h"
#include "Rev/Render/RHI/RHIShaderLibrary.h"
#include "Rev/Render/RenderPipeline/RenderPipeline.h"
#include "Rev/Render/RenderProxy/SceneRenderProxy.h"


namespace Rev
{


class SkyboxMaterial : public Material
{
public:
	SkyboxMaterial()
	{
		mDomain = MD_PostProcess;
	}
	virtual ~SkyboxMaterial() = default;

	virtual void Compile() override
	{
		mProgram = FRHIShaderLibrary::GetInstance().CreateGraphicsProgram(
			"SkyboxProgram",
			{ "/Engine/Shaders/Skybox/SkyboxVS" },
			{ "/Engine/Shaders/Skybox/SkyboxFS" }
		);
	}

	virtual void PreDraw() override
	{
		RenderCmd::EnableDepthWrite(false);
		Material::PreDraw();
	}

	virtual void PostDraw() override
	{
		Material::PostDraw();
		RenderCmd::EnableDepthWrite(true);
	}
};

FSkyboxPass::FSkyboxPass()
	: FFullScreenPass("SkyboxPass")
{
}

FSkyboxPass::~FSkyboxPass()
{
}

void FSkyboxPass::BeginPass()
{
	FFullScreenPass::BeginPass();
	if (!mMaterial)
	{
		mMaterial = CreateRef<SkyboxMaterial>();
		mMaterial->Compile();
	}
}

void FSkyboxPass::RunPass()
{
	const auto& SkyProxy = FRenderPipeline::GetCurrentPipeline()->GetSceneProxy()->GetSkyProxy();
	if (SkyProxy.GetSkybox())
	{
		mMaterial->PreDraw();
		DrawQuad();
		mMaterial->PostDraw();
	}
}

}


