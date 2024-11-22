#include "Rev/Render/RenderProxy/LightProxy.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Math/Maths.h"
#include "Rev/World/Entity.h"
#include "Rev/Render/UniformLayout.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHICommandList.h"
#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Render/Material/ShadowMapMaterial.h"

namespace Rev
{

FLightProxy::FLightProxy()
{
}

FLightProxy::~FLightProxy()
{
}

void FLightProxy::Prepare(const Ref<FScene>& Scene)
{
	{
		//Directional lights
		uint32 LightCount = 0;
		auto EntityView = Scene->EntityView<DirectionalLightComponent, TransformComponent>();
		for (const auto& [Entiny, LightComp, TransComp] : EntityView.each())
		{
			if (LightCount >= REV_MAX_DIRECTIONAL_LIGHTS)
				break;
			if (LightComp.Light.GetIntensity() <= 0)
				continue;

			uint32 NumShadowMapLayers = 1;

			auto& Light = DirectionalLightParams.Lights[LightCount];
			Light.Direction = TransComp.Transform.Up() * -1.0f;
			Light.Color = LightComp.Light.GetColor();
			Light.Intensity = LightComp.Light.GetIntensity();

			if (Light.ShadowMapCount != NumShadowMapLayers)
			{
				Light.ShadowMapCount = NumShadowMapLayers;
				if(NumShadowMapLayers == 0)
				{
					DirectionalShadowData[LightCount].Texture.reset();
				}
				else
				{
					FRHITextureDesc ShadowMapDesc = FRHITextureDesc::Create2DArray(REV_SHADOWMAP_SIZE, REV_SHADOWMAP_SIZE, NumShadowMapLayers, PF_ShadowDepth).SetClearColor(FRHITextureClearColor(1.0, 0)).SetFlags(ETextureCreateFlags::DepthStencilTarget);
					DirectionalShadowData[LightCount].Texture = GDynamicRHI->RHICreateTexture(ShadowMapDesc);
				}
			}

			Light.ProjMats[0] = Math::FMatrix4::Othographic(-10, 10, -10, 10, -10, 10);
			Light.ViewMats[0] = TransComp.Transform.ToMatrix().Inverse();

			LightCount++;
		}
		DirectionalLightParams.Count = LightCount;
	}

	if(!ShadowMapMat)
		ShadowMapMat = CreateRef<FShadowMapMaterial>();
	
}

void FLightProxy::SyncResource(FRHICommandList& RHICmdList)
{
	if (!mLightUB)
		mLightUB = GDynamicRHI->RHICreateUniformBuffer(sizeof(FDirectionalLightUniform));

	mLightUB->UpdateSubData(&DirectionalLightParams, sizeof(FDirectionalLightUniform));
	RHICmdList.GetContext()->RHIBindUniformBuffer(UL::BLight, mLightUB.get());

	//Bind DirectionalLight shadow map
}

FRHITexture* FLightProxy::GetDirectionalShadowMap(uint32 Index)
{
	if (Index >= DirectionalLightParams.Count)
		return nullptr;

	return DirectionalShadowData[Index].Texture.get();
}

void FLightProxy::BeginDrawDirectionalShadowMap(FRHICommandList& RHICmdList, uint32 Index)
{
	if(Index >= DirectionalLightParams.Count)
		return;

	if(!DirectionalShadowData[Index].Texture)
		return;

	if (!DirectionalShadowData[Index].ViewUB)
		DirectionalShadowData[Index].ViewUB = GDynamicRHI->RHICreateUniformBuffer(sizeof(FShadowViewUniform));
	DirectionalShadowData[Index].ViewUB->UpdateSubData(&DirectionalLightParams.Lights[Index], sizeof(FShadowViewUniform));

	RHICmdList.GetContext()->RHIBindUniformBuffer(UL::BShadowView, DirectionalShadowData[Index].ViewUB.get());
}

void FLightProxy::EndDrawDirectionalShadowMap()
{
}

}


