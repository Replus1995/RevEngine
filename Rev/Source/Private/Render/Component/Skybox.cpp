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
	RE_CORE_ASSERT(InTexture && InTexture->GetResource()->GetDesc().Dimension == ETextureDimension::TextureCube)
	mEnvTex = InTexture;
}

}