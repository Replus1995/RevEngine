#pragma once
#include "Rev/Core/Layer.h"
#include "Rev/Events/ApplicationEvent.h"
#include "Rev/Render/RenderProxy/SceneProxy.h"

namespace Rev
{

class REV_API SceneLayer : public Layer
{
public:
	SceneLayer(const std::string& name = "SceneLayer");
	SceneLayer(const Ref<FScene>& scene, const std::string& name = "SceneLayer");
	virtual ~SceneLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(float dt) override;
	virtual void OnEvent(Event& event) override;
	virtual void OnDraw(FRHICommandList& RHICmdList) override;

protected:
	Ref<FScene> mScene;
	Scope<FSceneProxy> mSceneProxy;
	Ref<class FRenderer> mRenderer;
};

}