#include "Rev/World/SceneLayer.h"
#include "Rev/World/Scene.h"
#include "Rev/Render/RenderCmd.h"
#include "Rev/Core/Application.h"
#include "Rev/Core/Window.h"

#include "Rev/Render/Renderer/ForwardRenderer.h"


namespace Rev
{
SceneLayer::SceneLayer(const std::string& name)
	: Layer(name)
{
}

SceneLayer::SceneLayer(const Ref<FScene>& scene, const std::string& name)
	: Layer(name)
	, mScene(scene)
{

}

SceneLayer::~SceneLayer()
{

}

void SceneLayer::OnAttach()
{
	mSceneProxy = CreateScope<FSceneProxy>();
	if (mScene)
	{
		mScene->OnRuntimeStart();
	}
	mRenderer = CreateRef<FForwardRenderer>(mSceneProxy.get());

	RenderCmd::SetClearColor(Math::FLinearColor(0, 0, 0));
}

void SceneLayer::OnDetach()
{
	mRenderer.reset();
	if (mScene)
	{
		mScene->OnRuntimeStop();
	}
	mSceneProxy->FreeResource();
}

void SceneLayer::OnUpdate(float dt)
{
	if (mScene)
	{
		mScene->OnUpdateRuntime(dt);
	}

	if(Application::GetApp().Minimized())
		return;
}

void SceneLayer::OnEvent(Event& event)
{
	EventDispatcher dispatcher(event);
}

void SceneLayer::OnDraw()
{
	mSceneProxy->Prepare(mScene);

	//uint32 WinWidth = Application::GetApp().GetWindow()->GetWidth();
	//uint32 WinHeight = Application::GetApp().GetWindow()->GetHeight();


	mRenderer->BeginFrame();
	mRenderer->DrawFrame();
	mRenderer->EndFrame();


	//RenderCmd::BeginFrame(true);
	//RenderCmd::SetViewport(0, 0, WinWidth, WinHeight);
	//RenderCmd::ClearBackBuffer();
	//RenderCmd::EndFrame();

	mSceneProxy->Cleanup();
}


}