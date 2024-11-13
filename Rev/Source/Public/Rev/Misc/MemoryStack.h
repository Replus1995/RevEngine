#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Templates/FixedSizeAllocator.h"

namespace Rev
{

class REV_API FPageAllocator
{
public:
	enum
	{
		PageSize = 64 * 1024 //64k page 
	};

	FPageAllocator() = default;
	~FPageAllocator() = default;

	void* Alloc();
	void Free(void* Memory);
	uint64 BytesUsed();
	uint64 BytesFree();

private:
	TFixedSizeAllocator<PageSize> InnerAllocator;

};
REV_API extern FPageAllocator GPageAllocator;

class REV_API FMemoryStack
{
public:
	FMemoryStack();
	FMemoryStack(const FMemoryStack&) = delete;
	FMemoryStack(FMemoryStack&& Other) noexcept;
	FMemoryStack& operator=(FMemoryStack&& Other) noexcept;

	~FMemoryStack();

	void* Alloc(size_t AllocSize, size_t Alignment);

	bool IsEmpty() const { return TopChunk == nullptr; }

	struct FTaggedChunk
	{
		FTaggedChunk* Next;
		uint32 DataSize;
		uint8* Data() const { return ((uint8*)this) + sizeof(FTaggedChunk); }
	};
private:
	void AllocChunk(uint32 MinSize);
	void FreeChunks(FTaggedChunk* NewTopChunk);


	uint8* Top; //Top of current chunk
	uint8* End; //End of current chunk
	FTaggedChunk* TopChunk;

};


}