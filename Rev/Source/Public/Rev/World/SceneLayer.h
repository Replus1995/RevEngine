#pragma once
#include "Rev/Core/Layer.h"
#include "Rev/Events/ApplicationEvent.h"
//#include "Rev/Render/Resource/FrameBuffer.h"
#include "Rev/Render/RenderProxy/SceneRenderProxy.h"

namespace Rev
{
class REV_API SceneLayer : public Layer
{
public:
	SceneLayer(const std::string& name = "SceneLayer");
	SceneLayer(const Ref<Scene>& scene, const std::string& name = "SceneLayer");
	virtual ~SceneLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(float dt) override;
	virtual void OnEvent(Event& event) override;

protected:
	Ref<Scene> mScene;
	SceneRenderProxy mSceneProxy;
	//Ref<Framebuffer> mFrameBuffer;
};

}