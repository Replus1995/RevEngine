#pragma once
#include "Pine/Render/MaterialUniform.h"
#include <unordered_map>

namespace Pine
{
class MaterialUniform;
class Material
{
public:
	Material(const Ref<Shader>& program);
	virtual ~Material();

	template<typename T>
	bool SetUniform(std::string_view name, const T& value)
	{
		if (MaterialUniform* pParam = FindParam(name); pParam)
		{
			return pParam->SetValue(value);
		}
		return false;
	}

	template<typename T>
	bool GetUniform(std::string_view name, T& value)
	{
		if (MaterialUniform* pParam = FindParam(name); pParam)
		{
			return pParam->GetValue(value);
		}
		return false;
	}

	void Bind();
	void Unbind();

protected:
	MaterialUniform* FindParam(std::string_view name);

protected:
	Ref<Shader> mProgram = nullptr;
	std::unordered_map<std::string_view, std::unique_ptr<MaterialUniform>> mParamMap;
};

}