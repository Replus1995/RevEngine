#include "Rev/Render/RHI/RHICommandList.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FRHICommandList::FRHICommandList(FRHIContext* InContext)
	: mContext(InContext)
{
	REV_CORE_ASSERT(mContext);
}

FRHICommandList::~FRHICommandList()
{
}

}


