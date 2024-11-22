#pragma once
#include "Rev/Render/FrameGraph.h"

namespace Rev
{

class FFGShadowPass
{
public:
	struct Data : public FFGPassData
	{
		FFGHandle DepthTex;
	};

	FFGShadowPass(FFrameGraph& InGraph, const FFGSetupData& InSetupData);
};

}