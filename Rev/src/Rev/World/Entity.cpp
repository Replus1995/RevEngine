#include "pinepch.h"
#include "Entity.h"
#include "Pine/World/Scene.h"

namespace Pine
{

Entity::Entity(entt::entity handle, Scene* scene)
	: mHandle(handle)
	, mScene(scene)
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