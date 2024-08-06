#include "Rev/Render/RenderProxy/SkyRenderProxy.h"

namespace Rev
{

SkyRenderProxy::SkyRenderProxy()
	: mSkybox(nullptr)
{
}

SkyRenderProxy::~SkyRenderProxy()
{
}

void SkyRenderProxy::Prepare(const Skybox* InSkybox)
{
	mSkybox = InSkybox;
}

void SkyRenderProxy::Cleanup()
{
	mSkybox = nullptr;
}


}