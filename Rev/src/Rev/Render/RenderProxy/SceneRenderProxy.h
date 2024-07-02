#pragma once
#include "Pine/Core/Base.h"
#include "Pine/Render/Material.h"
#include "Pine/Render/Resource/UniformBuffer.h"
#include "Pine/Render/RenderData/RenderData.h"

#include "Pine/Render/RenderProxy/StaticMeshRenderProxy.h"

namespace Pine
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
