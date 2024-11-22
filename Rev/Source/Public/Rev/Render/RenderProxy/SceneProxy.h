#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Utils/Rect2D.h"
#include "Rev/Render/RenderProxy/CameraProxy.h"
#include "Rev/Render/RenderProxy/StaticMeshProxy.h"
#include "Rev/Render/RenderProxy/LightProxy.h"
#include "Rev/Render/RenderProxy/SkyProxy.h"

namespace Rev
{

struct alignas(16) FSceneUniform
{
	FRect2D ViewExtent;
	Math::FVector4 ViewPos;
	Math::FMatrix4 ViewMat;
	Math::FMatrix4 ProjMat;
	Math::FMatrix4 ViewProjMat;
	Math::FMatrix4 InvViewMat;
	Math::FMatrix4 InvProjMat;
	Math::FMatrix4 InvViewProjMat;
};

class FRHIUniformBuffer;
class FScene;
class FSceneProxy
{
public:
	FSceneProxy() = default;
	~FSceneProxy() = default;


	void Prepare(const Ref<FScene>& Scene);
	void SyncResource(FRHICommandList& RHICmdList);
	void Cleanup();

	void DrawSceneOpaque(FRHICommandList& RHICmdList);
	void DrawSkybox(FRHICommandList& RHICmdList);

protected:
	FSceneUniform mSceneParams;
	Ref<FRHIUniformBuffer> mSceneUB;

	FCameraProxy mCameraProxy;
	FStaticMeshProxy mStaticMeshProxy;
	FLightProxy mLightProxy;
	FSkyProxy mSkyProxy;

};

}
