#pragma once
#include "Rev/Render/Material/MaterialUniform.h"
#include "Rev/Math/Maths.h"
#include <unordered_map>

namespace Rev
{

enum class EMaterialDomain
{
	Opaque,
	Transparent,
	Unlit
};

class MaterialUniform;
class REV_API Material
{
public:
	Material(const Ref<FRHIShaderProgram>& InProgram);
	virtual ~Material();
	
	const Ref<FRHIShaderProgram>& GetShaderProgram() const
	{
		return mProgram;
	}

	void SetDomain(EMaterialDomain domain);
	EMaterialDomain GetDomain() const;

	void Bind();
	void Unbind();

protected:
	virtual void UploadUniform();
protected:
	Ref<FRHIShaderProgram> mProgram = nullptr;
	EMaterialDomain mDomain = EMaterialDomain::Opaque;
};

}