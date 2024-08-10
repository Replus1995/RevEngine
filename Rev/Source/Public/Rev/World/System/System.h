#pragma once
#include "Rev/Core/Base.h"

#define DECLARE_SYSTEM(SysClass)										\
public:																	\
	SysClass(FScene* InScene) : ISystem(InScene) {}						\
	virtual Rev::Scope<ISystem> Duplicate(FScene* InScene) override		\
	{																	\
		return Rev::CreateScope<SysClass>(InScene);						\
	}																	\
private:
	

namespace Rev
{
class FScene;
class ISystem
{
public:
	virtual ~ISystem() = default;
	virtual Scope<ISystem> Duplicate(FScene* InScene) = 0;

	virtual void OnInit(){};
	virtual void OnUpdate(float dt){};
	virtual void OnDestroy(){};

protected:
	ISystem(FScene* InScene) : mScene(InScene) {}
	FScene* mScene = nullptr;
};

}
