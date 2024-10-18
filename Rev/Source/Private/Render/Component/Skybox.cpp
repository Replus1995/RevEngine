#include "Rev/Render/Component/Skybox.h"
#include "Rev/Render/Texture/Texture.h"
#include "Rev/Render/RHI/RHITexture.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

Skybox::Skybox()
{
}

Skybox::~Skybox()
{
}

const Ref<Texture>& Skybox::GetEnvironmentTexture() const
{
	return mEnvTex;
}

void Skybox::SetEnvironmentTexture(const Ref<Texture>& InTexture)
{
	REV_CORE_ASSERT(InTexture && InTexture->GetTextureRHI()->GetDesc().Dimension == ETextureDimension::TextureCube)
	mEnvTex = InTexture;
}

}