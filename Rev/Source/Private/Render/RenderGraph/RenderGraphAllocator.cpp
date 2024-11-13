#include "Rev/Render/RenderGraph/RenderGraphAllocator.h"

namespace Rev
{

FRGAllocator& Rev::FRGAllocator::Get()
{
    static FRGAllocator Instance;
    return Instance;
}

FRGAllocator::~FRGAllocator()
{
    ReleaseAll();
}

void FRGAllocator::ReleaseAll()
{
	for (int32 Index = TrackedList.size() - 1; Index >= 0; --Index)
	{
		TrackedList[Index]->~FTrackedAlloc();
	}
	TrackedList.clear();

	MemStack.Flush();
}

FRGAllocatorScope::~FRGAllocatorScope()
{
	Allocator.ReleaseAll();
}

}

