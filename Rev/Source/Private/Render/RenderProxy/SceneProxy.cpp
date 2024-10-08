#include "Rev/Render/RenderProxy/SceneProxy.h" 
#include "Rev/Render/RenderCmd.h"
#include "Rev/Render/RHI/RHICore.h"
#include "Rev/Render/UniformLayout.h"
#include "Rev/Render/Texture/Texture.h"

#include "Rev/Core/Application.h"
#include "Rev/Core/Window.h"

namespace Rev
{

void FSceneProxy::Prepare(const Ref<FScene>& Scene)
{
	if(!Scene) return;

	mCameraProxy.Prepare(Scene);
	mStaticMeshProxy.Prepare(Scene);
	mDirectionalLightProxy.Prepare(Scene);
	mSkyProxy.Prepare(Scene);
}

void FSceneProxy::SyncResource(const FRenderer* Renderer)
{

	//Should run on render thread
	//Update uniform buffer
	mCameraProxy.SyncResource(Renderer);
	mDirectionalLightProxy.SyncResource();
	mSkyProxy.SyncResource();
}

void FSceneProxy::FreeResource()
{
	mCameraProxy.FreeResource();
	mStaticMeshProxy.FreeResource();
	mDirectionalLightProxy.FreeResource();
}

void FSceneProxy::Cleanup()
{
	mStaticMeshProxy.Cleanup();
	mSkyProxy.Cleanup();
}

void FSceneProxy::DrawScene(const FRenderer* Renderer)
{
	mStaticMeshProxy.DrawMeshes(MBM_Opaque);
	mSkyProxy.DrawSkybox();
}

}
