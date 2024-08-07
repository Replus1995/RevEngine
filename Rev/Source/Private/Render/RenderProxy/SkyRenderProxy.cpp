#include "Rev/Render/RenderProxy/SkyRenderProxy.h"
#include "Rev/Render/UniformLayout.h"
#include "Rev/Render/RenderCmd.h"
#include "Rev/Render/Renderer.h"
#include "Rev/Render/Texture/Texture.h"
#include "Rev/Render/Material/Material.h"
#include "Rev/Render/RHI/RHIShaderLibrary.h"


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
		RenderCmd::SetDepthTestMode(DTM_LessEqual);
		Material::PreDraw();
	}

	virtual void PostDraw() override
	{
		Material::PostDraw();
		RenderCmd::SetDepthTestMode(DTM_Less);
		RenderCmd::EnableDepthWrite(true);
	}
};


SkyRenderProxy::SkyRenderProxy()
{
}

SkyRenderProxy::~SkyRenderProxy()
{
}

void SkyRenderProxy::Prepare(const Skybox& InSkybox)
{
	mSkybox = InSkybox;
}

void SkyRenderProxy::Cleanup()
{
	mSkybox = {};
}

void SkyRenderProxy::SyncResource() const
{
	if (auto& EnvTex = mSkybox.GetEnvironmentTexture(); EnvTex)
	{
		RenderCmd::BindTexture(EnvTex->GetResource(), UL::SEnviornmentTex);
	}
	if (!mSkyboxMat)
	{
		SkyRenderProxy* pThis = const_cast<SkyRenderProxy*>(this);
		pThis->mSkyboxMat = CreateRef<SkyboxMaterial>();
		pThis->mSkyboxMat->Compile();
	}
}

void SkyRenderProxy::DrawSkybox() const
{
	if (auto& EnvTex = mSkybox.GetEnvironmentTexture(); EnvTex)
	{
		mSkyboxMat->PreDraw();
		Renderer::DrawScreenQuad();
		mSkyboxMat->PostDraw();
	}
}


}