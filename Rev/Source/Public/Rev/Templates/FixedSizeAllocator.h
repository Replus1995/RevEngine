#pragma once
#include "Rev/Core/Base.h"
#include <vector>

namespace Rev
{

template<int32 SIZE>
class TFixedSizeAllocator
{
public:
	TFixedSizeAllocator() {}
	~TFixedSizeAllocator()
	{
		REV_CORE_ASSERT(NumUsed == 0);
		Trim();
	}

	void* Allocate()
	{
		NumUsed++;
		void* NewMemory = nullptr;
		if (!FreeList.empty())
		{
			NewMemory = FreeList.back();
			FreeList.pop_back();
		}
		else
		{
			NewMemory = malloc(SIZE);
		}
		return NewMemory;
	}


	void Free(void* Memory)
	{
		NumUsed--;
		FreeList.push_back(Memory);
		NumFree++;
	}

	void Trim()
	{
		while (NumFree > 0)
		{
			void* Memory = FreeList.back();
			FreeList.pop_back();
			free(Memory);
			NumFree--;
		}
	}

	uint32 GetNumUsed() const { return NumUsed; }
	uint32 GetNumFree() const { return NumFree; }

private:
	uint32 NumUsed;
	uint32 NumFree;
	std::vector<void*> FreeList; //TODO: Use linked list & memory pool to optimize this array
};

}