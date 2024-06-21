#pragma once
#include "Pine/Core/Layer.h"
#include "Pine/Events/ApplicationEvent.h"
//#include "Pine/Render/Resource/FrameBuffer.h"
#include "Pine/Render/RenderProxy/SceneRenderProxy.h"

namespace Pine
{
class PINE_API SceneLayer : public Layer
{
public:
	SceneLayer(const std::string& name = "SceneLayer");
	SceneLayer(const Ref<Scene>& scene, const std::string& name = "SceneLayer");
	virtual ~SceneLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(float dt) override;
	virtual void OnEvent(Event& event) override;

private:
	bool OnWindowResize(WindowResizeEvent& e);

private:
	Ref<Scene> mScene;
	SceneRenderProxy mSceneProxy;
	//Ref<Framebuffer> mFrameBuffer;
};

}