#include "Rev/World/SceneLayer.h"
#include "Rev/World/Scene.h"
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Render/RHI/RHICommandList.h"
#include "Rev/Core/Application.h"
#include "Rev/Core/Window.h"

#include "Rev/Render/Renderer.h"


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
	mRenderer = CreateRef<FRenderer>(mSceneProxy.get());
}

void SceneLayer::OnDetach()
{
	mRenderer.reset();
	if (mScene)
	{
		mScene->OnRuntimeStop();
		mScene->DestroyAllEntities();
	}
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

void SceneLayer::OnDraw(FRHICommandList& RHICmdList)
{
	mSceneProxy->Prepare(RHICmdList, mScene);

	//uint32 WinWidth = Application::GetApp().GetWindow()->GetWidth();
	//uint32 WinHeight = Application::GetApp().GetWindow()->GetHeight();
	RHICmdList.GetContext()->RHISetViewport(0, 0, mSceneProxy->GetFrameWidth(), mSceneProxy->GetFrameHeight());


	mRenderer->BeginFrame(RHICmdList);
	mRenderer->DrawFrame(RHICmdList);
	mRenderer->EndFrame(RHICmdList);


	//RenderCmd::BeginFrame(true);
	//RenderCmd::SetViewport(0, 0, WinWidth, WinHeight);
	//RenderCmd::ClearBackBuffer();
	//RenderCmd::EndFrame();

	mSceneProxy->Cleanup();
}


}