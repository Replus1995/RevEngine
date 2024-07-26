#include "Rev/Render/RenderProxy/SceneRenderProxy.h" 
#include "Rev/World/Scene.h"
#include "Rev/World/System/PlayerCameraSystem.h"
#include "Rev/Render/RenderCmd.h"
#include "Rev/Render/Renderer.h"
#include "Rev/Render/RHI/RHIResourceFactory.h"

//Private
#include "Render/UniformLocation.h"

namespace Rev
{
void SceneRenderProxy::Init()
{
	mCameraUB = FRHIResourceFactory::CreateUniformBuffer(sizeof(FCameraUniform), UBB_Camera);
	mModelUB = FRHIResourceFactory::CreateUniformBuffer(sizeof(FModelUniform), UBB_Model);
	mForwardLightUB = FRHIResourceFactory::CreateUniformBuffer(sizeof(FForwardLightUniform), UBB_ForwardLight);
}

void SceneRenderProxy::Release()
{
	mForwardLightUB.reset();
	mModelUB.reset();
	mCameraUB.reset();
}

void SceneRenderProxy::Prepare(const Ref<Scene>& scene)
{
	if(!scene) return;

	{
		//Update Scene Uniform
		PlayerCameraSystem* pSystem = scene->GetSystem<PlayerCameraSystem>();
		if (pSystem)
		{
			pSystem->FillCameraUniform(mCameraData);
		}
	}
	{
		//Collect static meshes
		mStaticMeshProxies.clear();
		auto group = scene->mRegistry.group<StaticMeshComponent>(entt::get<TransformComponent>);
		for (auto entity : group)
		{
			auto [transformComp, meshComp] = group.get<TransformComponent, StaticMeshComponent>(entity);
			StaticMeshRenderProxy proxy;
			proxy.Prepare(meshComp.StaticMesh, transformComp.Transform);
			mStaticMeshProxies.emplace_back(std::move(proxy));
		}
		//ToDo: sort static meshes by depth
	}
	{
		//Collect lights
		mLightProxies.clear();

		auto dlgroup = scene->mRegistry.group<DirectionalLightComponent>(entt::get<TransformComponent>);
		for (auto entity : dlgroup)
		{
			auto [transformComp, lightComp] = dlgroup.get<TransformComponent, DirectionalLightComponent>(entity);
			LightRenderProxy proxy;
			proxy.Prepare(lightComp.Light, transformComp.Transform);
			mLightProxies.emplace_back(std::move(proxy));
		}
		//TODO: Collect spot/point lights
	}
}

void SceneRenderProxy::DrawScene()
{
	RenderCmd::SetClearColor(Renderer::sClearColor);
	RenderCmd::Clear();

	mCameraUB->UpdateData(&mCameraData, sizeof(FCameraUniform));

	uint32 LightCount = Math::Min<uint32>(mLightProxies.size(), UNIFORM_MAX_FORWARD_LIGHTS);
	for (uint32 i = 0; i < LightCount; i++)
	{
		mForwardLightData.Lights[i] = mLightProxies[i].GetUnifiedLight();
	}
	mForwardLightData.LightCount = LightCount;
	mForwardLightUB->UpdateData(&mForwardLightData, sizeof(FForwardLightUniform));

	DrawMeshes(EMaterialDomain::MD_Surface, BM_Opaque);
}

void SceneRenderProxy::DrawMeshes(EMaterialDomain InDomain, EBlendMode InBlend)
{
	for (StaticMeshRenderProxy& proxy : mStaticMeshProxies)
	{
		//TODO: Cluster Forward Light
		mModelUB->UpdateData(&proxy.GetMatrix(), sizeof(FModelUniform));
		proxy.DrawPrimitives(InDomain, InBlend);
	}
}

}
