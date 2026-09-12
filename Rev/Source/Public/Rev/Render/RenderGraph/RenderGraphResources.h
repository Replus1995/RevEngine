#pragma once
#include "Rev/Render/RenderGraph/RenderGraphDefinitions.h"

namespace Rev
{
class REV_API FRGTransientResourcePool
{
public:
	FRHITextureRef AcquireTexture(const FRGTextureDesc& InDesc);
	void EndFrame();
	void Reset();
private:
	struct FEntry { FRGTextureDesc Desc; FRHITextureRef Texture; uint64 LastUsedFrame = 0; bool bInUse = false; };
	std::vector<FEntry> Textures;
	uint64 FrameSerial = 0;
	static constexpr uint64 FramesUntilReuse = 2;
};
}
