#include "Rev/Render/RenderProxy/LightRenderProxy.h"
#include "Rev/Math/Maths.h"

namespace Rev
{

static void ToUnifiedLight(FUnifiedLight& Result, const Light* InLight, const Math::FTransform& InTransform)
{
	Result.Type = (uint32)InLight->GetKind();
	Result.Position = InTransform.Location;
	Result.Direction = InTransform.Forward();
	Result.ShadowIndex = -1;
	Result.Color = { InLight->GetColor().R, InLight->GetColor().G, InLight->GetColor().B };
	Result.Intensity = InLight->GetIntensity();
}

LightRenderProxy::LightRenderProxy()
{
}

LightRenderProxy::~LightRenderProxy()
{
}

void LightRenderProxy::Prepare(const DirectionalLight& InLight, const Math::FTransform& InTransform)
{
	ToUnifiedLight(mLightData, &InLight, InTransform);
}

void LightRenderProxy::Prepare(const SpotLight& InLight, const Math::FTransform& InTransform)
{
	ToUnifiedLight(mLightData, &InLight, InTransform);
	mLightData.Params.X = InLight.GetRadius();
	mLightData.Params.Y = InLight.GetAngleRadians();
}

void LightRenderProxy::Prepare(const PointLight& InLight, const Math::FTransform& InTransform)
{
	ToUnifiedLight(mLightData, &InLight, InTransform);
	mLightData.Params.X = InLight.GetRadius();
}

}


