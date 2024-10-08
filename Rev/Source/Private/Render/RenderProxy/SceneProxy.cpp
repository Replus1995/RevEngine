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

void FSceneProxy::SyncResource()
{
	if(!mSceneUB)
		mSceneUB = FRHICore::CreateUniformBuffer(sizeof(FSceneUniform), UL::BScene);

	//Should run on render thread
	//Update uniform buffer
	mCameraProxy.SyncResource();
	mDirectionalLightProxy.SyncResource();
	mSkyProxy.SyncResource();

	{
		auto pWindow = Application::GetApp().GetWindow();

		//Update scene uniform
		mSceneParams.ViewExtent = {0, 0, pWindow->GetWidth(), pWindow->GetHeight()};
		mSceneParams.ViewPos = mCameraProxy.GetViewPos();
		mSceneParams.ViewMat = mCameraProxy.GetViewProjMat();
		mSceneParams.ProjMat = mCameraProxy.GetProjMat();
		mSceneParams.ViewProjMat = mCameraProxy.GetViewProjMat();
		mSceneParams.InvViewProjMat = mSceneParams.ViewProjMat.Inverse();

		mSceneUB->UpdateSubData(&mSceneParams, sizeof(FSceneUniform));
	}

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

	mSceneUB.reset();
}

void FSceneProxy::DrawScene()
{
	mStaticMeshProxy.DrawMeshes(MBM_Opaque);
	mSkyProxy.DrawSkybox();
}

}
