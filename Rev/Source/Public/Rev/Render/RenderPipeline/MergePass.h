#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RHI/RHIRenderPass.h"

namespace Rev
{

class FMergePass : public FRHIRenderPass
{
public:
	FMergePass();
	virtual ~FMergePass();

};

}