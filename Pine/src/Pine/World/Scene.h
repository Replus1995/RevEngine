#pragma once
#include "Pine/Core/Base.h"
#include "Pine/Core/UUID.h"
#include "Pine/World/Entity.h"

#include <entt/entt.hpp>

namespace Pine
{

class Camera;
class Entity;
class Scene
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

	bool IsRunning() const { return mIsRunning; }
	bool IsPaused() const { return mIsPaused; }
	void SetPaused(bool paused) { mIsPaused = paused; }

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

	friend class Entity;
	//friend class SceneSerializer;
	//friend class SceneHierarchyPanel;
};

}