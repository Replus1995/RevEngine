#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/UniformDefine.h"
#include "Rev/Render/Material/Material.h"
#include "Rev/Render/RenderProxy/CameraProxy.h"
#include "Rev/Render/RenderProxy/StaticMeshProxy.h"
#include "Rev/Render/RenderProxy/LightProxy.h"
#include "Rev/Render/RenderProxy/SkyProxy.h"

namespace Rev
{
class FRHIUniformBuffer;
class FScene;
class FSceneProxy
{
public:
	FSceneProxy() = default;
	~FSceneProxy() = default;


	void Prepare(const Ref<FScene>& Scene);
	void SyncResource(FRHICommandList& RHICmdList);
	void FreeResource();
	void Cleanup();

	void DrawSceneOpaque(FRHICommandList& RHICmdList);
	void DrawSkybox(FRHICommandList& RHICmdList);

	uint32 GetFrameWidth() const;
	uint32 GetFrameHeight() const;

protected:
	FSceneUniform mSceneParams;
	Ref<FRHIUniformBuffer> mSceneUB;

	FCameraProxy mCameraProxy;
	FStaticMeshProxy mStaticMeshProxy;
	FLightProxy mLightProxy;
	FSkyProxy mSkyProxy;

};

}
