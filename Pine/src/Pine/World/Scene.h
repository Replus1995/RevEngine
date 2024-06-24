#pragma once
#include "Pine/Core/Base.h"
#include "Pine/Core/UUID.h"
#include "Pine/World/Entity.h"
#include "Pine/World/System/System.h"

#include <entt/entt.hpp>

namespace Pine
{

class Camera;
class Entity;
class PINE_API Scene
{
public:
	Scene();
	~Scene();

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

	Entity CreateEntity(const std::string& name = std::string());
	Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
	void DestroyEntity(Entity entity);
	Entity DuplicateEntity(Entity entity);

	Entity FindEntityByName(std::string_view name);
	Entity FindEntityByUUID(UUID uuid);

	template<typename... Components>
	auto GetAllEntitiesWith()
	{
		return mRegistry.view<Components...>();
	}

	template<class TSystem>
	bool AddSystem()
	{
		size_t hash = typeid(TSystem).hash_code();
		if (auto iter = mSystemMap.find(hash); iter == mSystemMap.end())
		{
			mSystemMap.emplace(hash, std::make_unique<TSystem>(mRegistry));
			return true;
		}
		return false;
	}


	template<class TSystem>
	bool RemoveSystem()
	{
		size_t hash = typeid(TSystem).hash_code();
		if (auto iter = mSystemMap.find(hash); iter != mSystemMap.end())
		{
			mSystemMap.erase(iter);
			return true;
		}
		return false;
	}

	template<class TSystem>
	TSystem* GetSystem()
	{
		size_t hash = typeid(TSystem).hash_code();
		if (auto iter = mSystemMap.find(hash); iter != mSystemMap.end())
		{
			return (TSystem*)iter->second.get();
		}
		return nullptr;
	}


	//static Ref<World> Copy(Ref<World> other);
private:
	//template<typename T>
	//void OnComponentAdded(Entity entity, T& component);

	//void OnPhysics2DStart();
	//void OnPhysics2DStop();

	//void RenderScene(Camera& camera);
private:
	entt::registry mRegistry;
	uint32_t mViewWidth = 0, mViewHeight = 0;
	bool mIsRunning = false;
	bool mIsPaused = false;

	//b2World* m_PhysicsWorld = nullptr;

	std::unordered_map<UUID, entt::entity> mEntityMap;
	std::map<size_t, Scope<class ISystem>> mSystemMap;

	friend class Entity;
	friend class SceneRenderProxy;
	//friend class SceneSerializer;
	//friend class SceneHierarchyPanel;
};

}