#include "Rev/Render/RenderProxy/LightProxy.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Math/Maths.h"
#include "Rev/World/Entity.h"
#include "Rev/Render/UniformLayout.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHICommandList.h"
#include "Rev/Render/RHI/RHIBuffer.h"

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
	uint32 LightCount = 0;
	auto EntityView = Scene->EntityView<DirectionalLightComponent, TransformComponent>();
	for (const auto& [Entiny, LightComp, TransComp] : EntityView.each())
	{
		if (LightCount >= REV_MAX_DIRECTIONAL_LIGHTS)
			break;
		if (LightComp.Light.GetIntensity() <= 0)
			continue;
		auto& Light = mDirectionalLightParams.Lights[LightCount];
		Light.Direction = TransComp.Transform.Up() * -1.0f;
		Light.Color = LightComp.Light.GetColor();
		Light.Intensity = LightComp.Light.GetIntensity();
		Light.ShadowIndex = -1; //temp
		Light.ShadowCount = 0; //temp
		LightCount++;
	}
	mDirectionalLightParams.Count = LightCount;
}

void FLightProxy::SyncResource(FRHICommandList& RHICmdList)
{
	if (!mLightUB)
		mLightUB = GDynamicRHI->RHICreateUniformBuffer(sizeof(FDirectionalLightUniform));

	mLightUB->UpdateSubData(&mDirectionalLightParams, sizeof(FDirectionalLightUniform));
	RHICmdList.GetContext()->RHIBindUniformBuffer(UL::BLight, mLightUB.get());
}

}


