#include "Rev/Render/RHI/RHIState.h"

namespace Rev
{

bool operator==(const FSamplerStateDesc& A, const FSamplerStateDesc& B)
{
	bool bSame = A.Filter == B.Filter &&
		A.WarpU == B.WarpU &&
		A.WarpV == B.WarpV &&
		A.WarpW == B.WarpW &&
		A.MipBias == B.MipBias &&
		A.MinMipLevel == B.MinMipLevel &&
		A.MaxMipLevel == B.MaxMipLevel &&
		A.MaxAnisotropy == B.MaxAnisotropy && 
		A.BorderColor == B.BorderColor &&
		A.CompareFunc == B.CompareFunc;
	return bSame;
}

}