#pragma once
#include "Pine/Core/UUID.h"
#include "Pine/Core/Assert.h"
#include "Pine/World/Component/Component.h"

#include <entt/entt.hpp>

namespace Pine {

class Scene;
class PINE_API Entity
{
public:
	Entity() = default;
	Entity(entt::entity handle, Scene* scene);
	Entity(const Entity& other) = default;

	template<typename T, typename... Args>
	T& AddComponent(Args&&... args)
	{
		PE_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
		T& component = mScene->mRegistry.emplace<T>(mHandle, std::forward<Args>(args)...);
		return component;
	}

	template<typename T, typename... Args>
	T& AddOrReplaceComponent(Args&&... args)
	{
		T& component = mScene->mRegistry.emplace_or_replace<T>(mHandle, std::forward<Args>(args)...);
		return component;
	}

	template<typename T>
	T& GetComponent()
	{
		PE_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
		return mScene->mRegistry.get<T>(mHandle);
	}

	template<typename T>
	bool HasComponent()
	{
		return mScene->mRegistry.any_of<T>(mHandle);
	}

	template<typename T>
	void RemoveComponent()
	{
		PE_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
		mScene->mRegistry.remove<T>(mHandle);
	}

	operator bool() const { return mHandle != entt::null; }
	operator entt::entity() const { return mHandle; }
	operator uint32_t() const { return (uint32_t)mHandle; }

	UUID GetUUID();
	const std::string& GetName();

	bool operator==(const Entity& other) const
	{
		return mHandle == other.mHandle && mScene == other.mScene;
	}

	bool operator!=(const Entity& other) const
	{
		return !(*this == other);
	}
private:
	entt::entity mHandle{ entt::null };
	Scene* mScene = nullptr;

public:
	template<typename... Component>
	static void CopyComponentIfExists(Entity dst, Entity src)
	{
		([&]()
			{
				if (src.HasComponent<Component>())
					dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
			}(), ...);
	}

	template<typename... Component>
	static void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
	{
		CopyComponentIfExists<Component...>(dst, src);
	}
};



}