#pragma once
#include "Rev/Core/Layer.h"
#include "Rev/Events/ApplicationEvent.h"
//#include "Rev/Render/Resource/FrameBuffer.h"

namespace Rev
{

class REV_API RenderLayer : public Layer
{
public:
	RenderLayer();
	~RenderLayer();

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(float dt) override;
	void OnEvent(Event& event) override;

private:
	bool OnWindowResize(WindowResizeEvent& e);

private:
	//Ref<Framebuffer> mFrameBuffer;
};

}