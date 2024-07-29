#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RHI/RHIShader.h"

namespace Rev
{

template<typename T, uint16 Loc>
class REV_API FUniformRenderProxy
{
public:
	FUniformRenderProxy() = default;
	~FUniformRenderProxy() = default;

	FUniformRenderProxy& operator=(const T& InValue)
	{
		mValue = InValue;
		mDirty = true;
		return *this;
	}

	const T& Value() const
	{
		return mValue;
	}

	void Upload(const Ref<FRHIShaderProgram>& InShader)
	{
		if (mDirty && InShader != nullptr)
		{
			InShader->SetUniform(Loc, mValue);
			mDirty = false;
		}
	}

private:
	bool mDirty = false;
	T mValue;
};

}