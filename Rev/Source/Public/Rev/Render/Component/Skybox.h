#pragma once
#include "Rev/Core/Base.h"

namespace Rev
{
class FTexture;
class REV_API Skybox
{
public:
	Skybox();
	virtual ~Skybox();

	const Ref<FTexture>& GetEnvironmentTexture() const;
	void SetEnvironmentTexture(const Ref<FTexture>& InTexture);

private:
	Ref<FTexture> mEnvTex;
};

}