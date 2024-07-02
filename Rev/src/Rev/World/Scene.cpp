#include "pinepch.h"
#include "Scene.h"
#include "Pine/World/System/PlayerCameraSystem.h"

namespace Pine
{

Scene::Scene()
{
	AddSystem<PlayerCameraSystem>();
}

Scene::~Scene()
{
}

void Scene::OnRuntimeStart()
{
	mIsRunning = true;
	//OnPhysics2DStart();

	for (auto iter = mSystemMap.begin(); iter != mSystemMap.end(); iter++)
	{
		iter->second->OnInit();
	}

}

void Scene::OnRuntimeStop()
{
	mIsRunning = false;
	//OnPhysics2DStop();

	for (auto iter = mSystemMap.begin(); iter != mSystemMap.end(); iter++)
	{
		iter->second->OnDestroy();
	}
}

void Scene::OnUpdateRuntime(float dt)
{
	if (!mIsPaused)
	{
		for (auto iter = mSystemMap.begin(); iter != mSystemMap.end(); iter++)
		{
			iter->second->OnUpdate(dt);
		}
	}
}

Entity Scene::CreateEntity(const std::string& name)
{
	return CreateEntityWithUUID(UUID(), name);
}

Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
{
	Entity entity(mRegistry.create(), this);
	entity.AddComponent<IDComponent>(uuid);
	entity.AddComponent<TransformComponent>();
	auto& tag = entity.AddComponent<TagComponent>();
	tag.Tag = name.empty() ? "Entity" : name;

	mEntityMap[uuid] = entity;

	return entity;
}

void Scene::DestroyEntity(Entity entity)
{
	mEntityMap.erase(entity.GetUUID());
	mRegistry.destroy(entity);
}

Entity Scene::DuplicateEntity(Entity entity)
{
	std::string name = entity.GetName();
	Entity newEntity = CreateEntity(name);
	Entity::CopyComponentIfExists(AllComponents{}, newEntity, entity);
	return newEntity;
}

Entity Scene::FindEntityByName(std::string_view name)
{
	auto view = mRegistry.view<TagComponent>();
	for (auto entity : view)
	{
		const TagComponent& tc = view.get<TagComponent>(entity);
		if (tc.Tag == name)
			return Entity{ entity, this };
	}
	return {};
}

Entity Scene::FindEntityByUUID(UUID uuid)
{
	if (mEntityMap.find(uuid) != mEntityMap.end())
		return { mEntityMap.at(uuid), this };
	return {};
}

bool Scene::AddSystem(size_t hash, Scope<ISystem>&& pSystem)
{
	if (auto iter = mSystemMap.find(hash); iter == mSystemMap.end())
	{
		if(mIsRunning)
			pSystem->OnInit();
		mSystemMap.emplace(hash, std::move(pSystem));
		return true;
	}
	return false;
}

bool Scene::RemoveSystem(size_t hash)
{
	if (auto iter = mSystemMap.find(hash); iter != mSystemMap.end())
	{
		if (mIsRunning)
			iter->second->OnDestroy();
		mSystemMap.erase(iter);
		return true;
	}
	return false;
}

ISystem* Scene::GetSystem(size_t hash)
{
	if (auto iter = mSystemMap.find(hash); iter != mSystemMap.end())
	{
		return iter->second.get();
	}
	return nullptr;
}

}