#include "Rev/Render/RenderProxy/SceneRenderProxy.h" 
#include "Rev/World/Scene.h"
#include "Rev/World/System/PlayerCameraSystem.h"
#include "Rev/Render/RenderCmd.h"
#include "Rev/Render/Renderer.h"
#include "Rev/Render/RHI/RHIResourceFactory.h"
#include "Rev/Render/UniformLayout.h"
#include "Rev/Render/Texture/Texture.h"

#include "Rev/Core/Application.h"
#include "Rev/Core/Window.h"

namespace Rev
{
void SceneRenderProxy::Init()
{
	uCamera.Create();
	uScene.Create();
	uModel.Create();
	uForwardLight.Create();
}

void SceneRenderProxy::Release()
{
	uForwardLight.Destory();
	uModel.Destory();
	uScene.Destory();
	uCamera.Destory();
}

void SceneRenderProxy::Prepare(const Ref<Scene>& scene)
{
	if(!scene) return;

	{
		//Update Camera Uniform
		PlayerCameraSystem* pSystem = scene->GetSystem<PlayerCameraSystem>();
		if (pSystem)
		{
			pSystem->FillCameraUniform(uCamera.Data);
		}
	}
	{
		//Update Scene Uniform;
		auto pWindow = Application::GetApp().GetWindow();
		uScene.Data.ScreenWidth = pWindow->GetWidth();
		uScene.Data.ScreenHeight = pWindow->GetHeight();
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
		//TODO: Cluster Forward Light
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

		uint32 LightCount = Math::Min<uint32>(mLightProxies.size(), UNIFORM_MAX_FORWARD_LIGHTS);
		for (uint32 i = 0; i < LightCount; i++)
		{
			uForwardLight.Data.Lights[i] = mLightProxies[i].GetUnifiedLight();
		}
		uForwardLight.Data.LightCount = LightCount;
	}
	{
		//Sky
		auto SkyGroup = scene->mRegistry.group<SkyComponent>();
		if (!SkyGroup.empty())
		{
			auto Entity = SkyGroup[0];
			auto SkyComp = SkyGroup.get<SkyComponent>(Entity);
			mSkyProxy.Prepare(SkyComp.Skybox);
		}

	}
}

void SceneRenderProxy::Cleanup()
{
	mSkyProxy.Cleanup();
}

void SceneRenderProxy::DrawScene() const
{
	SyncResource();
	DrawStaticMeshes(BM_Opaque);
	DrawSkybox();
}

void SceneRenderProxy::SyncResource() const
{
	//Should run on render thread
	//Update uniform buffer
	uCamera.Upload();
	uScene.Upload();
	uForwardLight.Upload();
	mSkyProxy.SyncResource();
}

void SceneRenderProxy::DrawStaticMeshes(EBlendMode InBlend) const
{
	for (const StaticMeshRenderProxy& proxy : mStaticMeshProxies)
	{
		uModel.Upload(&proxy.GetMatrix(), sizeof(Math::FMatrix4));
		proxy.DrawPrimitives(InBlend);
	}
}

void SceneRenderProxy::DrawSkybox() const
{
	mSkyProxy.DrawSkybox();
}

}
