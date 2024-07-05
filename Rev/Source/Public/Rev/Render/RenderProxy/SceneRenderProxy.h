#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/Material/Material.h"
#include "Rev/Render/RHI/RHIUniformBuffer.h"
#include "Rev/Render/RenderData/RenderData.h"

#include "Rev/Render/RenderProxy/StaticMeshRenderProxy.h"

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
	void Draw();

protected:
	void DrawDomain(EMaterialDomain domain);

protected:
	SceneRenderData mSceneData;
	Ref<UniformBuffer> mSceneBuffer;

	std::vector<StaticMeshRenderProxy> mStaticMeshProxies;

};

}
