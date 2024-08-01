#pragma once
#include "Rev/Core/Base.h"

namespace Rev
{
class Texture;
class REV_API Skybox
{
public:
	Skybox();
	virtual ~Skybox();

	const Ref<Texture>& GetEnvironmentTexture();
	void SetEnvironmentTexture(const Ref<Texture>& InTexture);

private:
	Ref<Texture> mEnvTex;
};

}