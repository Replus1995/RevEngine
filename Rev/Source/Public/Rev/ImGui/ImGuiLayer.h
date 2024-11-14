#pragma once

#include "Rev/Core/Layer.h"
#include "Rev/Events/KeyEvent.h"
#include "Rev/Events/MouseEvent.h"
#include "Rev/Events/ApplicationEvent.h"

namespace Rev
{
	class REV_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(float dt) override;
		void OnEvent(Event& event) override;
		void OnDraw(FRHICommandList& RHICmdList) override;

	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnKeyReleased(KeyReleasedEvent& e);
		bool OnKeyTyped(KeyTypedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleased(MouseButtonReleasedEvent& e);
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnMouseMoved(MouseMovedEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		void* mPlatformData = nullptr;
	};
}