#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Core/UUID.h"
#include "Rev/World/System/System.h"

#include <entt/entt.hpp>

namespace Rev
{

class Camera;
class FEntity;
class ISystem;
class REV_API FScene
{
public:
	FScene();
	~FScene();

	void OnRuntimeStart();
	void OnRuntimeStop();

	//void OnSimulationStart();
	//void OnSimulationStop();

	void OnUpdateRuntime(float dt);
	//void OnUpdateSimulation(Timestep ts, EditorCamera& camera);
	//void OnUpdateEditor(Timestep ts, EditorCamera& camera);

	//void OnViewportResize(uint32_t width, uint32_t height);

	bool IsRunning() const { return mIsRunning; }
	bool IsPaused() const { return mIsPaused; }
	void SetPaused(bool paused) { mIsPaused = paused; }

	FEntity CreateEntity(const std::string& name = std::string());
	FEntity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
	void DestroyEntity(FEntity entity);
	void DestroyAllEntities();
	FEntity DuplicateEntity(FEntity entity);

	FEntity FindEntityByName(std::string_view name);
	FEntity FindEntityByUUID(UUID uuid);

	template<typename... TComponents>
	auto EntityView()
	{
		return mRegistry.view<TComponents...>();
	}

	template<typename... TComponents>
	auto EntityGroup()
	{
		return mRegistry.group<TComponents...>();
	}


	template<class TSystem>
	bool AddSystem()
	{
		size_t hash = typeid(TSystem).hash_code();
		return AddSystem(hash, std::move(CreateScope<TSystem>(this)));
	}


	template<class TSystem>
	bool RemoveSystem()
	{
		size_t hash = typeid(TSystem).hash_code();
		return RemoveSystem(hash);
	}

	template<class TSystem>
	TSystem* GetSystem()
	{
		size_t hash = typeid(TSystem).hash_code();
		return (TSystem*)GetSystem(hash);
	}


	//static Ref<World> Copy(Ref<World> other);
private:
	//template<typename T>
	//void OnComponentAdded(Entity entity, T& component);

	//void OnPhysics2DStart();
	//void OnPhysics2DStop();

	//void RenderScene(Camera& camera);

	bool AddSystem(size_t hash, Scope<ISystem>&& pSystem);
	bool RemoveSystem(size_t hash);
	ISystem* GetSystem(size_t hash);


private:
	entt::registry mRegistry;
	uint32_t mViewWidth = 0, mViewHeight = 0;
	bool mIsRunning = false;
	bool mIsPaused = false;

	//b2World* m_PhysicsWorld = nullptr;

	std::unordered_map<UUID, entt::entity> mEntityMap;
	std::map<size_t, Scope<ISystem>> mSystemMap;

	friend class FEntity;
	friend class FSceneProxy;
};

}