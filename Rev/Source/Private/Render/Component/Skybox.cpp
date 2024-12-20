#include "Rev/Render/Component/Skybox.h"
#include "Rev/Render/Resource/TextureResource.h"
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

const Ref<FTexture>& Skybox::GetEnvironmentTexture() const
{
	return mEnvTex;
}

void Skybox::SetEnvironmentTexture(const Ref<FTexture>& InTexture)
{
	REV_CORE_ASSERT(InTexture && InTexture->GetTextureRHI()->GetDesc().Dimension == ETextureDimension::TextureCube)
	mEnvTex = InTexture;
}

}