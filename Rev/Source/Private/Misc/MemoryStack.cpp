#include "Rev/Misc/MemoryStack.h"
#include "Rev/Templates/Alignment.h"

namespace Rev
{

FPageAllocator GPageAllocator;

void* FPageAllocator::Alloc()
{
    return InnerAllocator.Allocate();
}

void FPageAllocator::Free(void* Memory)
{
    InnerAllocator.Free(Memory);
}

uint64 FPageAllocator::BytesUsed()
{
    return InnerAllocator.GetNumUsed() * PageSize;
}

uint64 FPageAllocator::BytesFree()
{
    return InnerAllocator.GetNumFree() * PageSize;
}

FMemoryStack::FMemoryStack()
    : Top(nullptr)
    , End(nullptr)
    , TopChunk(nullptr)
{
}

FMemoryStack::FMemoryStack(FMemoryStack&& Other) noexcept
	: Top(Other.Top)
	, End(Other.End)
	, TopChunk(Other.TopChunk)
{
	Other.Top = nullptr;
	Other.End = nullptr;
	Other.TopChunk = nullptr;
}

FMemoryStack& FMemoryStack::operator=(FMemoryStack&& Other) noexcept
{
	Top = Other.Top;
	End = Other.End;
	TopChunk = Other.TopChunk;
	Other.Top = nullptr;
	Other.End = nullptr;
	Other.TopChunk = nullptr;
	return *this;
}

FMemoryStack::~FMemoryStack()
{
	FreeChunks(nullptr);
}

void* FMemoryStack::Alloc(size_t AllocSize, size_t Alignment)
{
	REV_CORE_ASSERT(AllocSize >= 0);
	REV_CORE_ASSERT((Alignment & (Alignment - 1)) == 0);
	REV_CORE_ASSERT(Top <= End);

	uint8* Result = Align(Top, Alignment);
	uint8* NewTop = Result + AllocSize;

	if (NewTop <= End)
	{
		Top = NewTop;
	}
	else
	{
		AllocChunk(AllocSize + Alignment);
		Result = Align(Top, Alignment);
		NewTop = Result + AllocSize;
		Top = NewTop;
	}
	return Result;
}

void FMemoryStack::AllocChunk(uint32 MinSize)
{
	FTaggedChunk* NewChunk = nullptr;
	uint32 TotalSize = MinSize + (uint32)sizeof(FTaggedChunk);
	uint32 AllocSize = AlignArbitrary<int32>(TotalSize, FPageAllocator::PageSize);
	if (AllocSize == FPageAllocator::PageSize)
	{
		NewChunk = (FTaggedChunk*)GPageAllocator.Alloc();
	}
	else
	{
		NewChunk = (FTaggedChunk*)malloc(AllocSize);
	}

	NewChunk->DataSize = AllocSize - sizeof(FTaggedChunk);

	NewChunk->Next = TopChunk;
	TopChunk = NewChunk;
	Top = NewChunk->Data();
	End = Top + NewChunk->DataSize;
}

void FMemoryStack::FreeChunks(FTaggedChunk* NewTopChunk)
{
	while (TopChunk != NewTopChunk)
	{
		FTaggedChunk* RemoveChunk = TopChunk;
		TopChunk = TopChunk->Next;
		if (RemoveChunk->DataSize + sizeof(FTaggedChunk) == FPageAllocator::PageSize)
		{
			GPageAllocator.Free(RemoveChunk);
		}
		else
		{
			free(RemoveChunk);
		}
	}
	Top = nullptr;
	End = nullptr;
	if (TopChunk)
	{
		Top = TopChunk->Data();
		End = Top + TopChunk->DataSize;
	}
}


}


