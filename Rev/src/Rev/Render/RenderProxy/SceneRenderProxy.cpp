#include "SceneRenderProxy.h" 
#include "Rev/World/Scene.h"
#include "Rev/World/System/PlayerCameraSystem.h"
#include "Rev/Render/RenderCmd.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Rev
{
void SceneRenderProxy::Init()
{
	mSceneBuffer = UniformBuffer::Create(sizeof(SceneRenderData), 0);
}

void SceneRenderProxy::Release()
{
	mSceneBuffer.reset();
}

void SceneRenderProxy::Prepare(const Ref<Scene>& scene)
{
	if(!scene) return;

	{
		//Update Scene Uniform
		PlayerCameraSystem* pSystem = scene->GetSystem<PlayerCameraSystem>();
		if (pSystem)
		{
			pSystem->GetCameraMatrix(mSceneData.ProjMatrix, mSceneData.ViewMatrix);
		}
	}
	{
		//Collect static meshes
		mStaticMeshProxies.clear();
		auto group = scene->mRegistry.group<TransformComponent>(entt::get<StaticMeshComponent>);
		for (auto entity : group)
		{
			auto [transformComp, meshComp] = group.get<TransformComponent, StaticMeshComponent>(entity);
			StaticMeshRenderProxy proxy;
			proxy.Prepare(meshComp.StaticMesh, transformComp.GetMatrix());
			mStaticMeshProxies.emplace_back(std::move(proxy));
		}
		//ToDo: sort static meshes by depth
	}
}

void SceneRenderProxy::Draw()
{
	RenderCmd::SetClearColor(Math:: FVector4{ .3f, .3f, .8f, 1.0f });
	RenderCmd::Clear();

	mSceneBuffer->SetData(&mSceneData, sizeof(SceneRenderData));

	DrawDomain(EMaterialDomain::Opaque);
	DrawDomain(EMaterialDomain::Transparent);
}

void SceneRenderProxy::DrawDomain(EMaterialDomain domain)
{
	for (StaticMeshRenderProxy& proxy : mStaticMeshProxies)
	{
		proxy.Draw(domain);
	}
}

}
