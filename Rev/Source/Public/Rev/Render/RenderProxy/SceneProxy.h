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


	void Prepare(const Ref<FScene>& scene);
	void SyncResource() const;
	void FreeResource();
	void Cleanup();

	void DrawScene() const;

protected:
	TUniform<FSceneUniform, UL::BScene> uScene;

	FCameraProxy mCameraProxy;
	FStaticMeshProxy mStaticMeshProxy;
	FDirectionalLightProxy mDirectionalLightProxy;
	FSkyProxy mSkyProxy;

};

}
