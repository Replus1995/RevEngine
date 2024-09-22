#include "Rev/Render/RenderProxy/CameraProxy.h"
#include "Rev/World/Scene.h"
#include "Rev/World/System/PlayerCameraSystem.h"

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
		pSystem->FillCameraUniform(uCamera.Data);
	}
}

void FCameraProxy::SyncResource() const
{
	uCamera.Upload();
}

void FCameraProxy::FreeResource()
{
	uCamera.FreeResource();
}

}