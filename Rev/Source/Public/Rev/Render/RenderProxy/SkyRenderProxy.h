#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/Component/Skybox.h"

namespace Rev
{

class SkyRenderProxy
{
public:
	SkyRenderProxy();
	~SkyRenderProxy();

	void Prepare(const Skybox* InSkybox);
	void Cleanup();

	const Skybox* GetSkybox() const { return mSkybox; }

private:
	const Skybox* mSkybox;
};

}