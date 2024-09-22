#include "Rev/Render/RHI/RHITexture.h"

namespace Rev
{

void FRHITexture::ClearAllData()
{
	for (uint8 i = 0; i < mDesc.NumMips; i++)
	{
		ClearMipData(i);
	}
}

}