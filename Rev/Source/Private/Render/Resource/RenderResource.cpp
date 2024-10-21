#include "Rev/Render/Resource/RenderResource.h"
#include "Rev/Core/Assert.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/RHI/RHITexture.h"

namespace Rev
{

class FRenderResourceList
{
public:
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
};

FRenderResourceList GGlobalResourceList;



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
		ListIndex = GGlobalResourceList.Allocate(this);
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
		GGlobalResourceList.Deallocate(ListIndex);
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
	GGlobalResourceList.ForEach([](FRenderResource* Resource)
		{
			Resource->InitRHI();
		});
}

void ReleaseGlobalResources()
{
	GGlobalResourceList.ForEach([](FRenderResource* Resource)
		{
			Resource->ReleaseRHI();
		});
}

uint32 FTexture::GetSizeX() const
{
	return TextureRHI ? TextureRHI->GetWidth() : 0;
}

uint32 FTexture::GetSizeY() const
{
	return TextureRHI ? TextureRHI->GetHeight() : 0;
}

uint32 FTexture::GetSizeZ() const
{
	return TextureRHI ? TextureRHI->GetDepth() : 0;
}

}