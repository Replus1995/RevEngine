#include "pinepch.h"
#include "Entity.h"
#include "Pine/World/World.h"

namespace Pine
{

Entity::Entity(entt::entity handle, World* world)
	: mHandle(handle)
	, mWorld(world)
{
}

UUID Entity::GetUUID()
{
	return GetComponent<IDComponent>().ID;
}

const std::string& Entity::GetName()
{
	return GetComponent<TagComponent>().Tag;
}

}