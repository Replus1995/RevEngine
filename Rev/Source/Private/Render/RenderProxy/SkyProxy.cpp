#include "Rev/Render/RenderProxy/SkyProxy.h"
#include "Rev/Render/UniformLayout.h"
#include "Rev/Render/RenderCmd.h"
#include "Rev/Render/RenderUtils.h"
#include "Rev/Render/Texture/Texture.h"
#include "Rev/Render/Material/Material.h"
#include "Rev/Render/RHI/RHIShaderLibrary.h"
#include "Rev/World/Entity.h"


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
		//RenderCmd::EnableDepthWrite(false);
		//RenderCmd::SetDepthTestMode(DTM_LessEqual);
		Material::PreDraw();
	}

	virtual void PostDraw() override
	{
		Material::PostDraw();
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

void FSkyProxy::SyncResource() const
{
	if (auto& EnvTex = mSkybox.GetEnvironmentTexture(); EnvTex)
	{
		RenderCmd::BindTexture(EnvTex->GetResource(), UL::SEnviornmentTex);
	}
	if (!mSkyboxMat)
	{
		FSkyProxy* pThis = const_cast<FSkyProxy*>(this);
		pThis->mSkyboxMat = CreateRef<SkyboxMaterial>();
		pThis->mSkyboxMat->Compile();
	}
}

void FSkyProxy::DrawSkybox() const
{
	if (auto& EnvTex = mSkybox.GetEnvironmentTexture(); EnvTex)
	{
		mSkyboxMat->PreDraw();
		RenderUtils::DrawScreenQuad();
		mSkyboxMat->PostDraw();
	}
}


}