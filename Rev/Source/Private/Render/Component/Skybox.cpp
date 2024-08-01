#include "Rev/Render/Component/Skybox.h"

namespace Rev
{

Skybox::Skybox()
{
}

Skybox::~Skybox()
{
}

const Ref<Texture>& Skybox::GetEnvironmentTexture()
{
	return mEnvTex;
}

void Skybox::SetEnvironmentTexture(const Ref<Texture>& InTexture)
{
	mEnvTex = InTexture;
}

}