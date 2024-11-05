#include "Rev/Render/RenderCore.h"
#include "Rev/Core/Assert.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHIShaderLibrary.h"
#include "Rev/Render/RHI/RHIPipeline.h"

namespace Rev
{

bool GIsRHIInitialized = false;

static ERenderAPI sRenderAPI = ERenderAPI::None;
static Scope<FRHIContext> sMainContext;

void FRenderCore::Init(ERenderAPI InAPI)
{
	REV_CORE_ASSERT(!GDynamicRHI, "RenderCmd already initialized!");
	sRenderAPI = InAPI;
	RHIInit(InAPI);
	sMainContext = GDynamicRHI->RHICreateContext();
	sMainContext->Init();
	FRHIShaderLibrary::Initialize(InAPI);
	FRHIPipelineStateCache::Initialize(InAPI);

	GIsRHIInitialized = true;
	
}

void FRenderCore::Cleanup()
{
	FRHIPipelineStateCache::Shutdown();
	FRHIShaderLibrary::Get()->ClearShadersCache();
	FRHIShaderLibrary::Shutdown();
	sMainContext->Cleanup();
	sMainContext.reset();
	RHIExit();

	GIsRHIInitialized = false;
}

ERenderAPI FRenderCore::GetRenderAPI()
{
	return sRenderAPI;
}

FRHIContext* FRenderCore::GetMainContext()
{
	return sMainContext.get();
}

}