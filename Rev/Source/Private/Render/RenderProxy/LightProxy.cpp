#include "Rev/Render/RenderProxy/LightProxy.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Math/Maths.h"
#include "Rev/World/Entity.h"

namespace Rev
{

FDirectionalLightProxy::FDirectionalLightProxy()
{
}

FDirectionalLightProxy::~FDirectionalLightProxy()
{
}

void FDirectionalLightProxy::Prepare(const Ref<FScene>& Scene)
{
	uint32 LightCount = 0;
	auto EntityView = Scene->EntityView<DirectionalLightComponent, TransformComponent>();
	for (const auto& [Entiny, LightComp, TransComp] : EntityView.each())
	{
		if (LightCount >= REV_MAX_DIRECTIONAL_LIGHTS)
			break;
		if (LightComp.Light.GetIntensity() <= 0)
			continue;
		auto& Info = uLight.Data.Lights[LightCount];
		Info.Direction = TransComp.Transform.Up() * -1.0f;
		Info.Color = LightComp.Light.GetColor();
		Info.Intensity = LightComp.Light.GetIntensity();
		Info.ShadowIndex = -1; //temp
		Info.ShadowCount = 0; //temp
		LightCount++;
	}
	uLight.Data.Count = LightCount;
}

void FDirectionalLightProxy::SyncResource() const
{
	uLight.Upload(16 + uLight.Data.Count * sizeof(FDirectionalLightUniform::Info));
	//uLight.Upload();
}

void FDirectionalLightProxy::FreeResource()
{
	uLight.FreeResource();
}

}


