#include "Rev/Render/RenderProxy/LightProxy.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Math/Maths.h"
#include "Rev/World/Entity.h"
#include "Rev/Render/UniformLayout.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHICommandList.h"
#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Core/Log.h"

#include <limits>

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
	uint32 ShadowCastingLightCount = 0;
	uint32 PrimaryShadowLightEntity = std::numeric_limits<uint32>::max();
	mHasShadowCastingDirectionalLight = false;
	auto EntityView = Scene->EntityView<DirectionalLightComponent, TransformComponent>();
	for (const auto& [Entity, LightComp, TransComp] : EntityView.each())
	{
		if (LightComp.Light.GetIntensity() <= 0 || !LightComp.Light.CastsShadows())
			continue;

		ShadowCastingLightCount++;
		PrimaryShadowLightEntity = Math::Min(PrimaryShadowLightEntity, uint32(Entity));
	}

	if (ShadowCastingLightCount > 1 && !mWarnedAboutMultipleShadowLights)
	{
		REV_CORE_WARN("Multiple directional lights cast shadows; selecting the lowest entity ID.");
		mWarnedAboutMultipleShadowLights = true;
	}
	else if (ShadowCastingLightCount <= 1)
	{
		mWarnedAboutMultipleShadowLights = false;
	}

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
		Light.ShadowIndex = -1;
		Light.ShadowCount = 0;
		if (uint32(Entiny) == PrimaryShadowLightEntity)
		{
			mHasShadowCastingDirectionalLight = true;
			mShadowDirection = Math::FVector3(Light.Direction);
			Light.ShadowIndex = 0;
		}
		LightCount++;
	}
	mDirectionalLightParams.Count = LightCount;
}

void FLightProxy::SyncResource(FRHICommandList& RHICmdList)
{
	if (!mLightUB)
		mLightUB = GDynamicRHI->RHICreateUniformBuffer(sizeof(FDirectionalLightUniform));

	mLightUB->UpdateSubData(&mDirectionalLightParams, sizeof(FDirectionalLightUniform));
	RHICmdList.BindUniformBuffer(UL::BLight, mLightUB.get());
}

void FLightProxy::SetShadowCascadeCount(uint32 InCount)
{
	for (uint32 Index = 0; Index < mDirectionalLightParams.Count; ++Index)
		if (mDirectionalLightParams.Lights[Index].ShadowIndex == 0)
			mDirectionalLightParams.Lights[Index].ShadowCount = InCount;
}

}


