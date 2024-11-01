#include "Rev/Render/RenderProxy/SceneProxy.h" 
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHICommandList.h"
#include "Rev/Render/RHI/RHIBuffer.h"
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
	mLightProxy.Prepare(Scene);
	mSkyProxy.Prepare(Scene);

	auto pWindow = Application::GetApp().GetWindow();
	mSceneParams.ViewExtent = { 0, 0, pWindow->GetWidth(), pWindow->GetHeight() };
}

void FSceneProxy::SyncResource(FRHICommandList& RHICmdList)
{
	if(!mSceneUB)
		mSceneUB = GDynamicRHI->RHICreateUniformBuffer(sizeof(FSceneUniform));

	//Should run on render thread
	//Update uniform buffer
	mCameraProxy.SyncResource(RHICmdList);
	mLightProxy.SyncResource(RHICmdList);
	mSkyProxy.SyncResource(RHICmdList);

	{
		
		mSceneParams.ViewPos = mCameraProxy.GetViewPos();
		mSceneParams.ViewMat = mCameraProxy.GetViewMat();
		mSceneParams.ProjMat = mCameraProxy.GetProjMat();
		mSceneParams.ViewProjMat = mCameraProxy.GetViewProjMat();
		mSceneParams.InvViewProjMat = mSceneParams.ViewProjMat.Inverse();

		mSceneUB->UpdateSubData(&mSceneParams, sizeof(FSceneUniform));

		RHICmdList.GetContext()->RHIBindUniformBuffer(UL::BScene, mSceneUB.get());
	}

}

void FSceneProxy::Cleanup()
{
	mStaticMeshProxy.Cleanup();
	mSkyProxy.Cleanup();
}

void FSceneProxy::DrawSceneOpaque(FRHICommandList& RHICmdList)
{
	mStaticMeshProxy.DrawMeshesOpaque(RHICmdList);
	
}

void FSceneProxy::DrawSkybox(FRHICommandList& RHICmdList)
{
	mSkyProxy.DrawSkybox(RHICmdList);
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
