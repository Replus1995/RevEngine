#include "pinepch.h"
#include "RenderLayer.h"
#include "Pine/Render/RenderCmd.h"

namespace Pine
{

RenderLayer::RenderLayer()
	: Layer("RenderLayer")
{

}

RenderLayer::~RenderLayer()
{

}

void RenderLayer::OnAttach()
{
	//prepare global resource
}

void RenderLayer::OnDetach()
{
	//delete global resource
}

void RenderLayer::OnUpdate(float dt)
{
	RenderCmd::SetClearColor(glm::vec4{ .3f, .3f, .8f, 1.0f });
	RenderCmd::Clear();
}

void RenderLayer::OnEvent(Event& event)
{
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<WindowResizeEvent>(PE_BIND_EVENT_FN(RenderLayer::OnWindowResize, this));
}

bool RenderLayer::OnWindowResize(WindowResizeEvent& e)
{
	return false;
}

}