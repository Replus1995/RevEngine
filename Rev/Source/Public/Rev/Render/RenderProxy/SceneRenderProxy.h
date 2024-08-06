#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/UniformDefine.h"
#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Render/Material/Material.h"
#include "Rev/Render/RenderProxy/StaticMeshRenderProxy.h"
#include "Rev/Render/RenderProxy/LightRenderProxy.h"
#include "Rev/Render/RenderProxy/SkyRenderProxy.h"

namespace Rev
{
class Scene;
class SceneRenderProxy
{
public:
	SceneRenderProxy() = default;
	~SceneRenderProxy() = default;

	void Init();
	void Release();

	void Prepare(const Ref<Scene>& scene);
	void Cleanup();
	void DrawScene() const;

	const std::vector<StaticMeshRenderProxy>& GetStaticMeshProxies() const { return mStaticMeshProxies; }
	const std::vector<LightRenderProxy>& GetLightProxies() const { return mLightProxies; }
	const SkyRenderProxy& GetSkyProxy() const { return mSkyProxy; }

protected:
	void DrawMeshes(EBlendMode InBlend) const;

protected:
	FCameraUniform mCameraData;
	Ref<FRHIUniformBuffer> mCameraUB;
	Ref<FRHIUniformBuffer> mModelUB;
	FForwardLightUniform mForwardLightData;
	Ref<FRHIUniformBuffer> mForwardLightUB;

	std::vector<StaticMeshRenderProxy> mStaticMeshProxies;
	std::vector<LightRenderProxy> mLightProxies;
	SkyRenderProxy mSkyProxy;

};

}
