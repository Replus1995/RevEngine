#include "Rev/Render/RenderProxy/SceneProxy.h" 
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHICommandList.h"
#include "Rev/Render/UniformLayout.h"

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
	//mSkyProxy.Prepare(Scene);

	auto pWindow = Application::GetApp().GetWindow();
	mSceneParams.ViewExtent = { 0, 0, pWindow->GetWidth(), pWindow->GetHeight() };
}

void FSceneProxy::SyncResource(FRHICommandList& RHICmdList)
{
	if(!mSceneUB)
		mSceneUB = GDynamicRHI->CreateUniformBuffer(sizeof(FSceneUniform));

	//Should run on render thread
	//Update uniform buffer
	mCameraProxy.SyncResource();
	mDirectionalLightProxy.SyncResource();
	//mSkyProxy.SyncResource();

	{
		
		mSceneParams.ViewPos = mCameraProxy.GetViewPos();
		mSceneParams.ViewMat = mCameraProxy.GetViewMat();
		mSceneParams.ProjMat = mCameraProxy.GetProjMat();
		mSceneParams.ViewProjMat = mCameraProxy.GetViewProjMat();
		mSceneParams.InvViewProjMat = mSceneParams.ViewProjMat.Inverse();

		mSceneUB->UpdateSubData(&mSceneParams, sizeof(FSceneUniform));

		RHICmdList.GetContext()->BindUniformBuffer(UL::BScene, mSceneUB.get());
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
	//mSkyProxy.Cleanup();
}

void FSceneProxy::DrawScene(FRHICommandList& RHICmdList)
{
	mStaticMeshProxy.DrawMeshesOpaque(RHICmdList);
	//mSkyProxy.DrawSkybox();
}

uint32 FSceneProxy::GetFrameWidth() const
{
	return mSceneParams.ViewExtent.Width;
}

uint32 FSceneProxy::GetFrameHeight() const
{
	return mSceneParams.ViewExtent.Height;
}

}
