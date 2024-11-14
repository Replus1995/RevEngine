#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Events/Event.h"

namespace Rev
{
class FRHICommandList;

class REV_API Layer
{
public:
	Layer(const std::string& name = "Layer");
	virtual ~Layer();

	virtual void OnAttach() {}
	virtual void OnDetach() {}
	virtual void OnUpdate(float deltaTime) {}
	virtual void OnEvent(Event& event) {}
	virtual void OnDraw(FRHICommandList& RHICmdList) {}

	inline const std::string& GetName() const { return mName; }
protected:
	std::string mName;
};

}
