#pragma once
#include "Pine/Core/Layer.h"
#include "Pine/Events/ApplicationEvent.h"
//#include "Pine/Render/Resource/FrameBuffer.h"

namespace Pine
{

class PINE_API RenderLayer : public Layer
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