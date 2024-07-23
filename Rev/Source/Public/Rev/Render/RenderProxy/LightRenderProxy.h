#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/UniformData.h"
#include "Rev/Render/Light.h"

namespace Rev
{
class LightRenderProxy
{
public:
	LightRenderProxy();
	~LightRenderProxy();

	void Prepare(const DirectionalLight& InLight, const Math::FTransform& InTransform);
	void Prepare(const SpotLight& InLight, const Math::FTransform& InTransform);
	void Prepare(const PointLight& InLight, const Math::FTransform& InTransform);

	const FUnifiedLight& GetUnifiedLight() const { return mLightData; }
private:
	FUnifiedLight mLightData;
};
}