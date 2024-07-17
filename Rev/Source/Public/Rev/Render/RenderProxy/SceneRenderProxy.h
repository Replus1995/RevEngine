#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Render/Material/Material.h"
#include "Rev/Render/RenderProxy/StaticMeshRenderProxy.h"

namespace Rev
{
class Scene;
struct SceneUniformData
{
	Math::FMatrix4 ProjMatrix;
	Math::FMatrix4 ViewMatrix;
};

class SceneRenderProxy
{
public:
	SceneRenderProxy() = default;
	~SceneRenderProxy() = default;

	void Init();
	void Release();

	void Prepare(const Ref<Scene>& scene);
	void DrawScene();

protected:
	void DrawMeshes(EMaterialDomain InDomain, EBlendMode InBlend);

protected:
	SceneUniformData mSceneData;
	Ref<FRHIUniformBuffer> mSceneUB;

	std::vector<StaticMeshRenderProxy> mStaticMeshProxies;

};

}
