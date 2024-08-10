#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/UniformDefine.h"
#include "Rev/Render/Component/Light.h"

namespace Rev
{

class FScene;
class FDirectionalLightProxy
{
public:
	FDirectionalLightProxy();
	~FDirectionalLightProxy();

	void Prepare(const Ref<FScene>& Scene);
	void SyncResource() const;
	void FreeResource();

private:
	TUniform<FDirectionalLightUniform, UL::BDirectionalLight> uLight;
};

}