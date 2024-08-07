#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/Component/Skybox.h"

namespace Rev
{
class Material;
class SkyRenderProxy
{
public:
	SkyRenderProxy();
	~SkyRenderProxy();

	void Prepare(const Skybox& InSkybox);
	void Cleanup();
	void SyncResource() const;
	void DrawSkybox() const;

private:
	Skybox mSkybox;
	Ref<Material> mSkyboxMat;
};

}