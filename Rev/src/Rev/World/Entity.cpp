#include "Entity.h"
#include "Rev/World/Scene.h"

namespace Rev
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