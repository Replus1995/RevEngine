#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/UniformDefine.h"
#include "Rev/Render/Component/Camera.h"

namespace Rev
{
class FScene;
class FCameraProxy
{
public:
	FCameraProxy();
	~FCameraProxy();

	void Prepare(const Ref<FScene>& Scene);
	void SyncResource() const;
	void FreeResource();

private:
	TUniform<FCameraUniform, UL::BCamera> uCamera;
};

}