#pragma once
#include "Rev/Render/FrameGraph.h"

namespace Rev
{

class FFGShadowPass
{
public:
    struct Data : public FFGPassData
    {
        FFGHandle ShadowMap;
    };

    FFGShadowPass(FFrameGraph& InGraph, const FFGViewData& InViewData, class FSceneProxy* InSceneProxy);
};

}
