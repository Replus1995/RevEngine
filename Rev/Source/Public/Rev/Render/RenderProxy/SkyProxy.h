#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/Component/Skybox.h"

namespace Rev
{
class FMaterial;
class FScene;
class FRHICommandList;
class FSkyProxy
{
public:
	FSkyProxy();
	~FSkyProxy();

	void Prepare(const Ref<FScene>& Scene);
	void Cleanup();
	void SyncResource(FRHICommandList& RHICmdList) const;

	void DrawSkybox(FRHICommandList& RHICmdList) const;

private:
	Skybox mSkybox;
	Ref<FMaterial> mSkyboxMat;
};

}