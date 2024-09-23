#include "Rev/Render/RenderProxy/SceneProxy.h" 
#include "Rev/Render/RenderCmd.h"
#include "Rev/Render/RHI/RHICore.h"
#include "Rev/Render/UniformLayout.h"
#include "Rev/Render/Texture/Texture.h"

#include "Rev/Core/Application.h"
#include "Rev/Core/Window.h"

namespace Rev
{

void FSceneProxy::Prepare(const Ref<FScene>& scene)
{
	if(!scene) return;

	{
		//Update Scene Uniform;
		auto pWindow = Application::GetApp().GetWindow();
		uScene.Data.ScreenWidth = pWindow->GetWidth();
		uScene.Data.ScreenHeight = pWindow->GetHeight();
	}
	mCameraProxy.Prepare(scene);
	mStaticMeshProxy.Prepare(scene);
	mDirectionalLightProxy.Prepare(scene);
	mSkyProxy.Prepare(scene);
}

void FSceneProxy::FreeResource()
{
	uScene.FreeResource();
	mCameraProxy.FreeResource();
	mStaticMeshProxy.FreeResource();
	mDirectionalLightProxy.FreeResource();
}

void FSceneProxy::Cleanup()
{
	mStaticMeshProxy.Cleanup();
	mSkyProxy.Cleanup();
}

void FSceneProxy::DrawScene() const
{
	mStaticMeshProxy.DrawMeshes(MBM_Opaque);
	mSkyProxy.DrawSkybox();
}

void FSceneProxy::SyncResource() const
{
	//Should run on render thread
	//Update uniform buffer
	uScene.Upload();
	mCameraProxy.SyncResource();
	mDirectionalLightProxy.SyncResource();
	mSkyProxy.SyncResource();
}

}
