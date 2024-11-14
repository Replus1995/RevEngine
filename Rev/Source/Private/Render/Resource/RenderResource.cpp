#include "Rev/Render/Resource/RenderResource.h"
#include "Rev/Core/Assert.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/RHI/RHITexture.h"

namespace Rev
{

class FRenderResourceList
{
public:
	static FRenderResourceList& GetInstance()
	{
		if(!Instance)
			Instance = new FRenderResourceList;
		return *Instance;
	}
	static void ReleaseInstance()
	{
		delete Instance;
		Instance = nullptr;
	}


	FRenderResourceList()
	{
	}
	~FRenderResourceList() = default;

	int32 Allocate(FRenderResource* Resource)
	{
		ResourceList.push_back(Resource);
		return int32(ResourceList.size());
	}

	void Deallocate(int32 ListIndex)
	{
		if(ListIndex <= 0)
			return;

		int32 RealIndex = ListIndex - 1;
		int32 NumResources = ResourceList.size();
		if (RealIndex < NumResources)
		{
			ResourceList[RealIndex] = nullptr;
		}
	}

	void Clear()
	{
		ResourceList.clear();
	}

	template<typename FunctionType>
	void ForEach(const FunctionType& Function)
	{
		REV_CORE_ASSERT(!bIsIterating);
		bIsIterating = true;
		int32 NumResources = ResourceList.size();
		for (int32 Index = 0; Index < NumResources; ++Index)
		{
			FRenderResource* Resource = ResourceList[Index];
			if (Resource)
			{
				REV_CORE_ASSERT(Resource->GetListIndex() == Index + 1);
				Function(Resource);
			}
		}
		bIsIterating = false;
	}

private:
	std::vector<FRenderResource*> ResourceList;
	bool bIsIterating = false;

	static FRenderResourceList* Instance;
};

FRenderResourceList* FRenderResourceList::Instance = nullptr;

FRenderResource::FRenderResource()
	: ListIndex(-1)
{
}

FRenderResource::~FRenderResource()
{
	REV_CORE_ASSERT(!IsInitialized())
}

void FRenderResource::InitResource()
{
	if(IsInitialized())
		return;

	if (!GIsRHIInitialized)
	{
		ListIndex = FRenderResourceList::GetInstance().Allocate(this);
	}
	else
	{
		ListIndex = 0;
		InitRHI();
	}
}

void FRenderResource::ReleaseResource()
{
	if (IsInitialized())
	{
		if(GIsRHIInitialized)
		{
			ReleaseRHI();
		}
		FRenderResourceList::GetInstance().Deallocate(ListIndex);
		ListIndex = -1;
	}
}

void FRenderResource::ReinitRHI()
{
	if (IsInitialized() && GIsRHIInitialized)
	{
		ReleaseRHI();
		InitRHI();
	}
}

void InitGlobalResources()
{
	FRenderResourceList::GetInstance().ForEach([](FRenderResource* Resource)
		{
			Resource->InitRHI();
		});
}

void ReleaseGlobalResources()
{
	FRenderResourceList::GetInstance().ForEach([](FRenderResource* Resource)
		{
			Resource->ReleaseRHI();
		});
	FRenderResourceList::ReleaseInstance();
}

}