#include "Rev/Render/RenderProxy/CameraProxy.h"
#include "Rev/World/Scene.h"
#include "Rev/World/System/PlayerCameraSystem.h"
#include "Rev/Render/Renderer/Renderer.h"

namespace Rev
{

FCameraProxy::FCameraProxy()
{
}

FCameraProxy::~FCameraProxy()
{
}

void FCameraProxy::Prepare(const Ref<FScene>& Scene)
{
	PlayerCameraSystem* pSystem = Scene->GetSystem<PlayerCameraSystem>();
	if (pSystem)
	{
		pSystem->FillCameraData(mViewPos, mViewMatrix, mProjMatrix);
	}
}

void FCameraProxy::SyncResource()
{
}

void FCameraProxy::FreeResource()
{
}

}