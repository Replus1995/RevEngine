#pragma once
#include "Pine/Render/MaterialParam.h"
#include <unordered_map>

namespace Pine
{
class MaterialParamBase;
class Material
{
public:
	Material(const Ref<Shader>& program);
	virtual ~Material();

	template<typename T>
	bool SetParam(std::string_view name, const T& value)
	{
		if (MaterialParamBase* pParam = FindParam(name); pParam)
		{
			return pParam->SetValue(value);
		}
		return false;
	}

	template<typename T>
	bool GetParam(std::string_view name, T& value)
	{
		if (MaterialParamBase* pParam = FindParam(name); pParam)
		{
			return pParam->GetParam(value);
		}
		return false;
	}

	void Bind();
	void Unbind();

protected:
	MaterialParamBase* FindParam(std::string_view name);

protected:
	Ref<Shader> mProgram = nullptr;
	std::unordered_map<std::string_view, MaterialParamBase*> mParamMap;
};

}