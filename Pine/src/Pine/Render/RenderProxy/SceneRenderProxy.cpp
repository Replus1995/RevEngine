#include "pinepch.h"
#include "SceneRenderProxy.h" 
#include "Pine/World/Scene.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Pine
{
void SceneRenderProxy::Init()
{
	mSceneBuffer = UniformBuffer::Create(sizeof(SceneRenderData), 0);
}

void SceneRenderProxy::Prepare(const Ref<Scene>& scene)
{
	{
		//Update Scene Uniform
		Entity primCamEntity = scene->GetPrimaryCamera();
		auto [transformComp, cameraComp] = scene->mRegistry.get<TransformComponent, CameraComponent>(primCamEntity);
		mSceneData.ProjMatrix = cameraComp.Camera.GetProjectionMatrix();
		mSceneData.ViewMatrix = glm::inverse(transformComp.GetTransform());
	}
	{
		//Collect static meshes
		mStaticMeshProxies.clear();
		auto group = scene->mRegistry.group<TransformComponent>(entt::get<StaticMeshComponent>);
		for (auto entity : group)
		{
			auto [transformComp, meshComp] = group.get<TransformComponent, StaticMeshComponent>(entity);
			StaticMeshRenderProxy proxy;
			proxy.Prepare(meshComp.StaticMesh, transformComp.GetTransform());
			mStaticMeshProxies.emplace_back(std::move(proxy));
		}
		//ToDo: sort static meshes by depth
	}
}

void SceneRenderProxy::Draw()
{
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
