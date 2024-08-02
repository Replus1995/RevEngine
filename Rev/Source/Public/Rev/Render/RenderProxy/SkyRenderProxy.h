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

	void Prepare(const Skybox& InSkybox);

	const Skybox& GetSkybox() const { return mSkybox; }

private:
	Skybox mSkybox;
};

}