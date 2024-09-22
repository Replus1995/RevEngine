#pragma once
#include "Rev/World/Component/BaseComponent.h"
#include "Rev/World/Component/LightComponent.h"
#include "Rev/World/Component/SkyComponent.h"
#include "Rev/World/Component/MeshComponent.h"
#include <entt/entt.hpp>

namespace Rev
{

template<typename... Component>
struct ComponentGroup
{
};

using AllComponents = ComponentGroup<
	TransformComponent, CameraComponent,
	DirectionalLightComponent, SpotLightComponent, PointLightComponent, 
	SkyComponent,
	StaticMeshComponent
>;

class ComponentUtils
{
public:

	template<typename... Component>
	static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		([&]()
			{
				auto view = src.view<Component>();
				for (auto srcEntity : view)
				{
					entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).ID);

					auto& srcComponent = src.get<Component>(srcEntity);
					dst.emplace_or_replace<Component>(dstEntity, srcComponent);
				}
			}(), ...);
	}

	template<typename... Component>
	static void CopyComponent(ComponentGroup<Component...>, entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		CopyComponent<Component...>(dst, src, enttMap);
	}
};

}