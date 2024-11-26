#pragma once
#include "Rev/Render/FrameGraph.h"

namespace Rev
{

class FFGSkyPass
{
public:
	struct Data : public FFGPassData
	{
		FFGHandle ColorTex;
		FFGHandle DepthTex;
	};

	FFGSkyPass(FFrameGraph& InGraph, const FFGSetupData& InViewData);
};

}