#pragma once
#include "Pine/Core/Base.h"
#include <entt/entt.hpp>

#define DECLARE_SYSTEM(sysClass)									\
public:																\
	sysClass(entt::registry& reg) : ISystem(reg) {}					\
	virtual Scope<ISystem> Duplicate(entt::registry& reg) override	\
	{																\
		return std::make_unique<sysClass>(reg);						\
	}																\
private:
	

namespace Pine
{

class ISystem
{
public:
	virtual ~ISystem() = default;
	virtual Scope<ISystem> Duplicate(entt::registry& reg) = 0;
protected:
	ISystem(entt::registry& reg) : mRegistry(reg) {}
	entt::registry& mRegistry;
};

}
