#include "Rev/Render/RenderCore.h"
#include "Rev/Core/Assert.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHIShaderLibrary.h"

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
	FRHIShaderLibrary::CreateInstance();

	GIsRHIInitialized = true;

	sMainContext->RHISetClearColor(Math::FLinearColor(0, 0, 0, 1));
	
}

void FRenderCore::Cleanup()
{
	FRHIShaderLibrary::GetInstance().ClearShadersCache();
	FRHIShaderLibrary::ReleaseInstance();
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