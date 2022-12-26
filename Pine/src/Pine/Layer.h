#pragma once

#include "Pine/Core.h"
#include "Pine/Events/Event.h"

namespace Pine
{
	class PINE_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return mDebugName; }
	protected:
		std::string mDebugName;
	};
}
