#include "Rev/Render/RenderProxy/SkyProxy.h"
#include "Rev/Render/UniformLayout.h"
#include "Rev/Render/RenderUtils.h"
#include "Rev/Render/Resource/RenderResource.h"
#include "Rev/Render/Resource/TextureResource.h"
#include "Rev/Render/Material/Material.h"
#include "Rev/Render/RHI/RHIShaderLibrary.h"
#include "Rev/Render/RHI/RHICommandList.h"
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/World/Entity.h"


namespace Rev
{


class SkyboxMaterial : public FMaterial
{
public:
	SkyboxMaterial()
	{
		Domain = MD_PostProcess;
	}
	virtual ~SkyboxMaterial() = default;

	virtual void Compile() override
	{
		ShaderProgram = FRHIShaderLibrary::Get()->CreateGraphicsProgram(
			"SkyboxProgram",
			{ "/Engine/Shaders/SkyboxVS" },
			{ "/Engine/Shaders/SkyboxPS" }
		);
	}

	virtual void PreDraw(FRHICommandList& RHICmdList) override
	{
		//RenderCmd::EnableDepthWrite(false);
		//RenderCmd::SetDepthTestMode(DTM_LessEqual);
		FMaterial::PreDraw(RHICmdList);
	}

	virtual void PostDraw(FRHICommandList& RHICmdList) override
	{
		FMaterial::PostDraw(RHICmdList);
	}
};


FSkyProxy::FSkyProxy()
{
}

FSkyProxy::~FSkyProxy()
{
}

void FSkyProxy::Prepare(const Ref<FScene>& Scene)
{
	auto EntityView = Scene->EntityView<SkyComponent>();
	if (!EntityView.empty())
	{
		const SkyComponent& SkyComp = EntityView.get<SkyComponent>(EntityView[0]);
		mSkybox = SkyComp.Skybox;
	}
}

void FSkyProxy::Cleanup()
{
	mSkybox = {};
}

void FSkyProxy::SyncResource(FRHICommandList& RHICmdList) const
{
	if (!mSkyboxMat)
	{
		FSkyProxy* pThis = const_cast<FSkyProxy*>(this);
		pThis->mSkyboxMat = CreateRef<SkyboxMaterial>();
		pThis->mSkyboxMat->Compile();
	}
	if (auto& EnvTex = mSkybox.GetEnvironmentTexture(); EnvTex)
	{
		RHICmdList.GetContext()->RHIBindTexture(TB::EnviornmentTex, EnvTex->GetTextureRHI(), EnvTex->GetSamplerStateRHI());
	}
}

void FSkyProxy::DrawSkybox(FRHICommandList& RHICmdList) const
{
	if (auto& EnvTex = mSkybox.GetEnvironmentTexture(); EnvTex)
	{
		mSkyboxMat->PreDraw(RHICmdList);
		FRenderUtils::PostProcessDraw(RHICmdList);
		mSkyboxMat->PostDraw(RHICmdList);
	}
}


}