#pragma once
#include "Rev/Core/UUID.h"
#include "Rev/Core/Assert.h"
#include "Rev/World/Component/AllComponents.h"
#include "Rev/World/Scene.h"

#include <entt/entt.hpp>

namespace Rev {

class FScene;
class REV_API FEntity
{
public:
	FEntity() = default;
	FEntity(entt::entity handle, FScene* scene);
	FEntity(const FEntity& other) = default;

	template<typename T, typename... Args>
	T& AddComponent(Args&&... args)
	{
		REV_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
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
		REV_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
		return mScene->mRegistry.get<T>(mHandle);
	}

	template<typename T>
	const T& GetComponent() const
	{
		REV_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
		return mScene->mRegistry.get<T>(mHandle);
	}
	
	template<typename... T>
	auto GetComponents()
	{
		REV_CORE_ASSERT(HasComponent<T...>(), "Entity does not have component!");
		return mScene->mRegistry.get<T...>(mHandle);
	}

	template<typename... T>
	auto GetComponents() const
	{
		REV_CORE_ASSERT(HasComponent<T...>(), "Entity does not have component!");
		return mScene->mRegistry.get<T...>(mHandle);
	}

	template<typename... T>
	bool HasComponent() const
	{
		return mScene->mRegistry.any_of<T...>(mHandle);
	}

	template<typename T>
	void RemoveComponent()
	{
		REV_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
		mScene->mRegistry.remove<T>(mHandle);
	}

	operator bool() const { return mHandle != entt::null; }
	operator entt::entity() const { return mHandle; }
	operator uint32_t() const { return (uint32_t)mHandle; }

	UUID GetUUID();
	const std::string& GetName();

	bool operator==(const FEntity& other) const
	{
		return mHandle == other.mHandle && mScene == other.mScene;
	}

	bool operator!=(const FEntity& other) const
	{
		return !(*this == other);
	}
private:
	entt::entity mHandle{ entt::null };
	FScene* mScene = nullptr;

public:
	template<typename... Component>
	static void CopyComponentIfExists(FEntity dst, FEntity src)
	{
		([&]()
			{
				if (src.HasComponent<Component>())
					dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
			}(), ...);
	}

	template<typename... Component>
	static void CopyComponentIfExists(ComponentGroup<Component...>, FEntity dst, FEntity src)
	{
		CopyComponentIfExists<Component...>(dst, src);
	}
};



}