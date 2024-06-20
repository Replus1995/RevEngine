#pragma once
#include "Pine/Render/MaterialUniform.h"
#include <unordered_map>

namespace Pine
{
enum class EMaterialDomain
{
	Opaque,
	Transparent,
	Unlit
};

class MaterialUniform;
class Material
{
public:
	Material(const Ref<Shader>& program);
	virtual ~Material();
	
	/*const Ref<Shader>& GetShader() const
	{
		return mProgram;
	}*/

	void SetDomain(EMaterialDomain domain)
	{
		mDomain = domain;
	}

	EMaterialDomain GetDomain() const
	{
		return mDomain;
	}

	template<typename T>
	bool SetUniform(std::string_view name, const T& value)
	{
		if (MaterialUniform* pParam = FindUIniform(name); pParam)
		{
			return pParam->SetValue(value);
		}
		return false;
	}

	template<typename T>
	bool GetUniform(std::string_view name, T& value)
	{
		if (MaterialUniform* pParam = FindUIniform(name); pParam)
		{
			return pParam->GetValue(value);
		}
		return false;
	}

	void Bind();
	void Unbind();

protected:
	MaterialUniform* FindUIniform(std::string_view name);
	virtual void UploadUniform();

protected:
	Ref<Shader> mProgram = nullptr;
	EMaterialDomain mDomain = EMaterialDomain::Opaque;
	std::unordered_map<std::string_view, std::unique_ptr<MaterialUniform>> mUniformMap;
};

}