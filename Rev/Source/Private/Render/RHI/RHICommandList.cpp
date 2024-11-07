#include "Rev/Render/RHI/RHICommandList.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FRHICommandList::FRHICommandList(IRHIContext* InContext)
	: mContext(InContext)
{
	REV_CORE_ASSERT(mContext);
}

FRHICommandList::~FRHICommandList()
{
}

}


