#include "Rev/World/Entity.h"
#include "Rev/World/Scene.h"

namespace Rev
{

FEntity::FEntity(entt::entity handle, FScene* scene)
	: mHandle(handle)
	, mScene(scene)
{
}

UUID FEntity::GetUUID()
{
	return GetComponent<IDComponent>().ID;
}

const std::string& FEntity::GetName()
{
	return GetComponent<TagComponent>().Tag;
}

}