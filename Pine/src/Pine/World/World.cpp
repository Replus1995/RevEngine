#include "pinepch.h"
#include "World.h"

namespace Pine
{

World::World()
{
}

World::~World()
{
}

void World::OnRuntimeStart()
{
	mIsRunning = true;
	//OnPhysics2DStart();
}

void World::OnRuntimeStop()
{
	mIsRunning = false;
	//OnPhysics2DStop();
}

void World::OnUpdateRuntime(float dt)
{
	if (!mIsPaused)
	{
		
	}
}

Entity World::CreateEntity(const std::string& name)
{
	return CreateEntityWithUUID(UUID(), name);
}

Entity World::CreateEntityWithUUID(UUID uuid, const std::string& name)
{
	Entity entity(mRegistry.create(), this);
	entity.AddComponent<IDComponent>(uuid);
	entity.AddComponent<TransformComponent>();
	auto& tag = entity.AddComponent<TagComponent>();
	tag.Tag = name.empty() ? "Entity" : name;

	mEntityMap[uuid] = entity;

	return entity;
}

void World::DestroyEntity(Entity entity)
{
	mEntityMap.erase(entity.GetUUID());
	mRegistry.destroy(entity);
}

Entity World::DuplicateEntity(Entity entity)
{
	std::string name = entity.GetName();
	Entity newEntity = CreateEntity(name);
	Entity::CopyComponentIfExists(AllComponents{}, newEntity, entity);
	return newEntity;
}

Entity World::FindEntityByName(std::string_view name)
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

Entity World::FindEntityByUUID(UUID uuid)
{
	if (mEntityMap.find(uuid) != mEntityMap.end())
		return { mEntityMap.at(uuid), this };
	return {};
}

}