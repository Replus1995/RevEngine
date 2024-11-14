#pragma once
#include "Rev/Core/Base.h"

namespace Rev
{

class FRenderOptions
{
	friend class FRenderCore;
public:
	void REV_API SetNumSamples(uint8 InNumSamples);

public:
	uint8 REV_API GetNumSamples() const { return NumSamples; }


private:
	bool bRenderGraphNeedRebuild = false;
	uint8 NumSamples = 1;
};

REV_API extern FRenderOptions GRenderOptions;

}