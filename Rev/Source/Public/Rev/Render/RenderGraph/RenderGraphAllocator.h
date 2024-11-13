#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Misc/MemoryStack.h"

namespace Rev
{

class REV_API FRGAllocator final
{
public:
	static FRGAllocator& Get();
	~FRGAllocator();

	FORCEINLINE void* Alloc(uint64 SizeInBytes, uint64 AlignInBytes)
	{
		return MemStack.Alloc(SizeInBytes, AlignInBytes);
	}

	template <typename T, typename... TArgs>
	FORCEINLINE T* Alloc(TArgs&&... Args)
	{
		void* Memory = Alloc(sizeof(T), alignof(T));
		TTrackedAlloc<T>* TrackedAlloc = new(Memory) TTrackedAlloc<T>(std::forward<TArgs&&>(Args)...);
		TrackedList.push_back(TrackedAlloc);
		return &TrackedAlloc->Alloc;
	}

private:
	FRGAllocator() = default;
	FRGAllocator(FRGAllocator&&) = default;
	FRGAllocator& operator=(FRGAllocator&&) = default;

	void ReleaseAll();

	FMemoryStack MemStack;

	struct FTrackedAlloc
	{
		virtual ~FTrackedAlloc() = default;
	};

	template <typename T>
	struct TTrackedAlloc final : FTrackedAlloc
	{
		template <typename... TArgs>
		FORCEINLINE TTrackedAlloc(TArgs&&... Args)
			: RealAlloc(std::forward<TArgs&&>(Args)...)
		{}

		T RealAlloc;
	};

	std::vector<FTrackedAlloc*> TrackedList;

	friend class FRGAllocatorScope;
};

class REV_API FRGAllocatorScope
{
public:
	FRGAllocatorScope()
		: Allocator(FRGAllocator::Get())
	{}

	~FRGAllocatorScope();

protected:
	FRGAllocator& Allocator;
};

}