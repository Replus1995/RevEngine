#include "ShadowRendering.h"
#include "Rev/Render/RenderProxy/SceneProxy.h"

namespace Rev
{

FFGShadowPass::FFGShadowPass(FFrameGraph& InGraph, const FFGSetupData& InSetupData)
{
	auto LightProxy = InSetupData.SceneProxy->GetLightProxy();
	uint32 DirectionalLightCount = LightProxy->GetDirectionalLightCount();

	for (uint32 i = 0; i < DirectionalLightCount; i++)
	{
		FFGHandle ShadowDepthTex = InGraph.Import("ShadowDepthTexture", FFGTextureExternal::Desc{}, FFGTextureExternal(LightProxy->GetDirectionalShadowMap(i)));

		InGraph.AddPass<FFGShadowPass::Data>(
			"Blit Pass",
			[&](FFGBuilder& InBuilder, FFGShadowPass::Data& InData) {
				InData.DepthTex = InBuilder.Write(ShadowDepthTex);
			},
			[=](const FFGShadowPass::Data& InData, FFGPassResources& InResources, FFGExecuteData& InContextData) {

				auto& RHICmdList = InContextData.RHICmdList;

			},
			false
		);
	}
}

}