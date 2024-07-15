#include "Rev/Render/RenderProxy/SceneRenderProxy.h" 
#include "Rev/World/Scene.h"
#include "Rev/World/System/PlayerCameraSystem.h"
#include "Rev/Render/RenderCmd.h"
#include "Rev/Render/Renderer.h"
#include "Rev/Render/RHI/RHIResourceFactory.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Rev
{
void SceneRenderProxy::Init()
{
	mSceneBuffer = FRHIResourceFactory::CreateUniformBuffer(sizeof(SceneRenderData), 0);
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
	RenderCmd::SetClearColor(Renderer::sClearColor);
	RenderCmd::Clear();

	mSceneBuffer->UpdateData(&mSceneData, sizeof(SceneRenderData));

	DrawDomain(EMaterialDomain::MD_Surface);
}

void SceneRenderProxy::DrawDomain(EMaterialDomain domain)
{
	for (StaticMeshRenderProxy& proxy : mStaticMeshProxies)
	{
		proxy.Draw(domain);
	}
}

}
