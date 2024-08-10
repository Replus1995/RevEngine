#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/Component/Skybox.h"

namespace Rev
{
class Material;
class FScene;
class FSkyProxy
{
public:
	FSkyProxy();
	~FSkyProxy();

	void Prepare(const Ref<FScene>& Scene);
	void Cleanup();
	void SyncResource() const;

	void DrawSkybox() const;

private:
	Skybox mSkybox;
	Ref<Material> mSkyboxMat;
};

}