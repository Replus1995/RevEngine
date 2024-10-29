#pragma once
#include "Rev/Core/Base.h"
#include <memory>

#define REV_MAX_TEXCOORDS 4

#define REV_MAX_DIRECTIONAL_LIGHTS 4
#define REV_MAX_POINT_LIGHTS 16
#define REV_MAX_SPOT_LIGHTS 16

#define REV_BINDING_OFFSET_BUFFER 0
#define REV_BINDING_OFFSET_SAMPLER 16
#define REV_BINDING_OFFSET_TEXTURE 32

namespace Rev
{

enum class ERenderAPI : uint8
{
    None = 0,
    Vulkan = 1
};
REV_API ERenderAPI GetRenderAPI();

extern REV_API bool GIsRHIInitialized;

class FRHIContext;
class REV_API FRenderCore
{
public:
    static void Init(ERenderAPI InAPI);
    static void Cleanup();

    static ERenderAPI GetRenderAPI();
    static FRHIContext* GetMainContext();
};

}
