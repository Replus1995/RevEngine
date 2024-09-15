#include "Rev/Render/PixelFormat.h"

namespace Rev
{

bool FPixelFormatUtils::HasDepth(EPixelFormat InFormat)
{
    return InFormat == PF_DepthStencil || InFormat == PF_ShadowDepth;
}

bool FPixelFormatUtils::HasStencil(EPixelFormat InFormat)
{
    return InFormat == PF_DepthStencil;
}

}


