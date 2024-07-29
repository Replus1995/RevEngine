#include "Rev/World/SceneLayer.h"
#include "Rev/World/Scene.h"
#include "Rev/Render/RenderCmd.h"
#include "Rev/Render/Renderer.h"
#include "Rev/Core/Application.h"
#include "Rev/Core/Window.h"

#include "Rev/Render/RenderPipeline/ForwardPipeline.h"

namespace Rev
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
	mRenderPipeline = CreateRef<FForwardPipeline>();
}

void SceneLayer::OnDetach()
{
	mRenderPipeline.reset();
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
	//mSceneProxy.DrawScene();

	uint32 WinWidth = Application::GetApp().GetWindow()->GetWidth();
	uint32 WinHeight = Application::GetApp().GetWindow()->GetHeight();

	mRenderPipeline->BeginPipeline(WinWidth, WinHeight);
	mRenderPipeline->RunPipeline(mSceneProxy);
	mRenderPipeline->EndPipeline();
}

void SceneLayer::OnEvent(Event& event)
{
	EventDispatcher dispatcher(event);
}


}