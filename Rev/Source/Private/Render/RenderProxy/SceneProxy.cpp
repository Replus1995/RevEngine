#include "Rev/Render/RenderProxy/SceneProxy.h" 
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Render/RHI/RHICommandList.h"
#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Render/UniformLayout.h"

namespace Rev
{

void FSceneProxy::Prepare(const Ref<FScene>& Scene)
{
	if(!Scene) return;

	mCameraProxy.Prepare(Scene);
	mStaticMeshProxy.Prepare(Scene);
	mLightProxy.Prepare(Scene);
	mSkyProxy.Prepare(Scene);
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
		mSceneParams.ViewExtent = { 0, 0, RHICmdList.GetContext()->RHIGetFrameWidth(), RHICmdList.GetContext()->RHIGetFrameHeight() };
		mSceneParams.ViewPos = mCameraProxy.GetViewPos();
		mSceneParams.ViewMat = mCameraProxy.GetViewMat();
		mSceneParams.ProjMat = mCameraProxy.GetProjMat();
		mSceneParams.ViewProjMat = mCameraProxy.GetViewProjMat();
		mSceneParams.InvViewMat = mSceneParams.ViewMat.Inverse();
		mSceneParams.InvProjMat = mSceneParams.ProjMat.Inverse();
		mSceneParams.InvViewProjMat = mSceneParams.ViewProjMat.Inverse();

		mSceneUB->UpdateSubData(&mSceneParams, sizeof(FSceneUniform));

		RHICmdList.GetContext()->RHIBindUniformBuffer(UB::Scene, mSceneUB.get());
	}

	RHICmdList.GetContext()->RHISetViewport(0, 0, RHICmdList.GetContext()->RHIGetFrameWidth(), RHICmdList.GetContext()->RHIGetFrameHeight());
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

void FSceneProxy::DrawShadowMaps(FRHICommandList& RHICmdList)
{
	RHICmdList.GetContext()->RHIBeginDebugLabel("ShadowMap Passes", Math::FLinearColor(0.6,0.6,0.6));

	RHICmdList.GetContext()->RHISetViewport(0, 0, REV_SHADOWMAP_SIZE, REV_SHADOWMAP_SIZE);
	for (uint32 i = 0; i < mLightProxy.GetDirectionalLightCount(); i++)
	{
		mLightProxy.BeginDirectionalShadowPass(RHICmdList, i);

		mStaticMeshProxy.DrawMeshesOpaque(RHICmdList, false);

		mLightProxy.EndShadowPass(RHICmdList);
	}

	RHICmdList.GetContext()->RHIEndDebugLabel();
}

}
