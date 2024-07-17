#include "Rev/Render/RenderProxy/SceneRenderProxy.h" 
#include "Rev/World/Scene.h"
#include "Rev/World/System/PlayerCameraSystem.h"
#include "Rev/Render/RenderCmd.h"
#include "Rev/Render/Renderer.h"
#include "Rev/Render/RHI/RHIResourceFactory.h"

#include "../UniformLocation.hpp"

namespace Rev
{
void SceneRenderProxy::Init()
{
	mSceneUB = FRHIResourceFactory::CreateUniformBuffer(sizeof(SceneUniformData), UBB_Scene);
}

void SceneRenderProxy::Release()
{
	mSceneUB.reset();
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

void SceneRenderProxy::DrawScene()
{
	RenderCmd::SetClearColor(Renderer::sClearColor);
	RenderCmd::Clear();

	mSceneUB->UpdateData(&mSceneData, sizeof(SceneUniformData));

	DrawMeshes(EMaterialDomain::MD_Surface, BM_Opaque);
}

void SceneRenderProxy::DrawMeshes(EMaterialDomain InDomain, EBlendMode InBlend)
{
	for (StaticMeshRenderProxy& proxy : mStaticMeshProxies)
	{
		proxy.DrawColored(InDomain, InBlend);
	}
}

}
