#pragma once
#include "Rev/Core/Base.h"

namespace Rev
{

struct FCSMSettings
{
	uint8 CascadeCount = 4;
	uint16 Resolution = 2048;
	float MaxDistance = 200.0f;
	float SplitLambda = 0.7f;
	float TransitionFraction = 0.1f;
	float CasterExtrusion = 50.0f;
	float DepthBias = -1.25f;
	float SlopeBias = -1.75f;
	float NormalBias = 0.002f;
};

class FRenderOptions
{
	friend class FRenderCore;
public:
	void REV_API SetNumSamples(uint8 InNumSamples);
	void REV_API SetCSMSettings(const FCSMSettings& InSettings);

public:
	uint8 REV_API GetNumSamples() const { return NumSamples; }
	REV_API const FCSMSettings& GetCSMSettings() const { return CSMSettings; }


private:
	bool bRenderGraphNeedRebuild = false;
	uint8 NumSamples = 1;
	FCSMSettings CSMSettings;
};

REV_API extern FRenderOptions GRenderOptions;

}
