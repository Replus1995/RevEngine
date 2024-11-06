#include "Rev/Render/RenderOptions.h"

namespace Rev
{
FRenderOptions GRenderOptions;

void FRenderOptions::SetNumSamples(uint8 InNumSamples)
{
	NumSamples = InNumSamples;
	bRenderGraphNeedRebuild = true;
}

}


