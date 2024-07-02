#include "pinepch.h"
#include "SceneLayer.h"
#include "Pine/World/Scene.h"
#include "Pine/Render/RenderCmd.h"
#include "Pine/Render/Renderer.h"
#include "Pine/Core/Application.h"

namespace Pine
{
SceneLayer::SceneLayer(const std::string& name)
	: Layer(name)
{
}

SceneLayer::SceneLayer(const Ref<Scene>& scene, const std::string& name)
	: Layer(name)
	, mScene(scene)
{

}

SceneLayer::~SceneLayer()
{

}

void SceneLayer::OnAttach()
{
	mSceneProxy.Init();
	if (mScene)
	{
		mScene->OnRuntimeStart();
	}
}

void SceneLayer::OnDetach()
{
	if (mScene)
	{
		mScene->OnRuntimeStop();
	}
	mSceneProxy.Release();
}

void SceneLayer::OnUpdate(float dt)
{
	if (mScene)
	{
		mScene->OnUpdateRuntime(dt);
	}

	if(Application::GetApp().Minimized())
		return;

	//Render Scene
	mSceneProxy.Prepare(mScene);
	mSceneProxy.Draw();
}

void SceneLayer::OnEvent(Event& event)
{
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<WindowResizeEvent>(PE_BIND_EVENT_FN(SceneLayer::OnWindowResize, this));
}

bool SceneLayer::OnWindowResize(WindowResizeEvent& e)
{
	Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
	return false;
}


}