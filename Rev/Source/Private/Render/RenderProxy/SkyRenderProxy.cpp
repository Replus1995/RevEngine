#include "Rev/Render/RenderProxy/SkyRenderProxy.h"

namespace Rev
{

SkyRenderProxy::SkyRenderProxy()
{
}

SkyRenderProxy::~SkyRenderProxy()
{
}

void SkyRenderProxy::Prepare(const Skybox& InSkybox)
{
	mSkybox = InSkybox;
}

}