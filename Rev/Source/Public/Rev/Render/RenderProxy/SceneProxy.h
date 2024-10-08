#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/UniformDefine.h"
#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Render/Material/Material.h"
#include "Rev/Render/RenderProxy/CameraProxy.h"
#include "Rev/Render/RenderProxy/StaticMeshProxy.h"
#include "Rev/Render/RenderProxy/LightProxy.h"
#include "Rev/Render/RenderProxy/SkyProxy.h"

namespace Rev
{
class FScene;
class FSceneProxy
{
public:
	FSceneProxy() = default;
	~FSceneProxy() = default;


	void Prepare(const Ref<FScene>& Scene);
	void SyncResource();
	void FreeResource();
	void Cleanup();

	void DrawScene();

protected:
	FSceneUniform mSceneParams;
	Ref<FRHIUniformBuffer> mSceneUB;

	FCameraProxy mCameraProxy;
	FStaticMeshProxy mStaticMeshProxy;
	FDirectionalLightProxy mDirectionalLightProxy;
	FSkyProxy mSkyProxy;

};

}
