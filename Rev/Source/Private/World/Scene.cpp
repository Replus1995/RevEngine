#include "Rev/World/Scene.h"
#include "Rev/World/System/PlayerCameraSystem.h"

namespace Rev
{

FScene::FScene()
{
	AddSystem<PlayerCameraSystem>();
}

FScene::~FScene()
{
}

void FScene::OnRuntimeStart()
{
	mIsRunning = true;
	//OnPhysics2DStart();

	for (auto iter = mSystemMap.begin(); iter != mSystemMap.end(); iter++)
	{
		iter->second->OnInit();
	}

}

void FScene::OnRuntimeStop()
{
	mIsRunning = false;
	//OnPhysics2DStop();

	for (auto iter = mSystemMap.begin(); iter != mSystemMap.end(); iter++)
	{
		iter->second->OnDestroy();
	}
}

void FScene::OnUpdateRuntime(float dt)
{
	if (!mIsPaused)
	{
		for (auto iter = mSystemMap.begin(); iter != mSystemMap.end(); iter++)
		{
			iter->second->OnUpdate(dt);
		}
	}
}

FEntity FScene::CreateEntity(const std::string& name)
{
	return CreateEntityWithUUID(UUID(), name);
}

FEntity FScene::CreateEntityWithUUID(UUID uuid, const std::string& name)
{
	FEntity entity(mRegistry.create(), this);
	entity.AddComponent<IDComponent>(uuid);
	entity.AddComponent<TransformComponent>();
	auto& tag = entity.AddComponent<TagComponent>();
	tag.Tag = name.empty() ? "Entity" : name;

	mEntityMap[uuid] = entity;

	return entity;
}

void FScene::DestroyEntity(FEntity entity)
{
	mEntityMap.erase(entity.GetUUID());
	mRegistry.destroy(entity);
}

FEntity FScene::DuplicateEntity(FEntity entity)
{
	std::string name = entity.GetName();
	FEntity newEntity = CreateEntity(name);
	FEntity::CopyComponentIfExists(AllComponents{}, newEntity, entity);
	return newEntity;
}

FEntity FScene::FindEntityByName(std::string_view name)
{
	auto view = mRegistry.view<TagComponent>();
	for (auto entity : view)
	{
		const TagComponent& tc = view.get<TagComponent>(entity);
		if (tc.Tag == name)
			return FEntity{ entity, this };
	}
	return {};
}

FEntity FScene::FindEntityByUUID(UUID uuid)
{
	if (mEntityMap.find(uuid) != mEntityMap.end())
		return { mEntityMap.at(uuid), this };
	return {};
}

bool FScene::AddSystem(size_t hash, Scope<ISystem>&& pSystem)
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

bool FScene::RemoveSystem(size_t hash)
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

ISystem* FScene::GetSystem(size_t hash)
{
	if (auto iter = mSystemMap.find(hash); iter != mSystemMap.end())
	{
		return iter->second.get();
	}
	return nullptr;
}

}