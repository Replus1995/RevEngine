#include "Rev/Render/RenderOptions.h"
#include <algorithm>

namespace Rev
{
FRenderOptions GRenderOptions;

void FRenderOptions::SetNumSamples(uint8 InNumSamples)
{
	NumSamples = InNumSamples;
	bRenderGraphNeedRebuild = true;
}

void FRenderOptions::SetDirectionalShadowMode(EDirectionalShadowMode InMode)
{
	DirectionalShadowMode = InMode;
	bRenderGraphNeedRebuild = true;
}

void FRenderOptions::SetCSMSettings(const FCSMSettings& InSettings)
{
	CSMSettings = InSettings;
	CSMSettings.CascadeCount = std::clamp<uint8>(CSMSettings.CascadeCount, 1, 4);
	CSMSettings.Resolution = std::max<uint16>(CSMSettings.Resolution, 1);
	CSMSettings.MaxDistance = std::max(CSMSettings.MaxDistance, 0.01f);
	CSMSettings.SplitLambda = std::clamp(CSMSettings.SplitLambda, 0.0f, 1.0f);
	CSMSettings.TransitionFraction = std::clamp(CSMSettings.TransitionFraction, 0.0f, 1.0f);
	CSMSettings.CasterExtrusion = std::max(CSMSettings.CasterExtrusion, 0.0f);
	bRenderGraphNeedRebuild = true;
}

}


