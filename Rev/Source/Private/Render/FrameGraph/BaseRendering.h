#pragma once
#include "Rev/Render/FrameGraph.h"

namespace Rev
{

class FFGBasePass
{
public:
	struct Data : public FFGPassData
	{
		FFGHandle ColorTex;
		FFGHandle ColorTexMS;
		FFGHandle DepthTex;
		FFGHandle DepthTexMS;
	};

	FFGBasePass(FFrameGraph& InGraph, const FFGSetupData& InViewData);
};


class FFGBlitPass
{
public:
	struct Data : public FFGPassData
	{
		FFGHandle ColorTex;
		FFGHandle BackTex;
	};

	FFGBlitPass(FFrameGraph& InGraph, const FFGSetupData& InViewData);
};

}