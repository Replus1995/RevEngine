#include "Rev/Render/Texture/Texture2D.h"

namespace Rev
{



Texture2D::Texture2D()
	: mRHITexture(nullptr)
	, mRHISampler(nullptr)
{
}

Texture2D::~Texture2D()
{
}

}