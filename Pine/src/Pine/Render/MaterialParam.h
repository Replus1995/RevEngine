#pragma once
#include "Pine/Render/Resource/Shader.h"

namespace Pine
{

struct MaterialParamBase
{
	template<typename T>
	bool SetValue(const T& value)
	{
		return SetValue(&T, sizeof(T));
	}

	template<typename T>
	bool GetValue(T& value)
	{
		return GetValue(&T, sizeof(T));
	}

	virtual void Upload(Shader* pProgram) = 0;

protected:
	virtual bool SetValue(const void* data, size_t size) = 0;
	virtual bool GetValue(void* data, size_t size) = 0;

	std::string mName;
};

template<typename T>
struct MaterialParam : public MaterialParamBase
{
	virtual void Upload(Shader* pProgram)
	{
		pProgram->SetUniform(mName, mValue);
	}

	T mValue;

protected:
	virtual bool SetValue(const void* data, size_t size)
	{
		if(size != sizeof(T))
			return false;
		mValue = *(const T*)data;
		return true;
	}
	virtual bool GetValue(void* data, size_t size)
	{
		if (size != sizeof(T))
			return false;
		*(T*)data = mValue;
		return true;
	}
};

template<typename T, size_t N>
struct MaterialParam<std::array<T, N>> : public MaterialParamBase
{
	virtual void Upload(Shader* pProgram)
	{
		pProgram->SetUniformArray(mName, mValueArr.data(), N);
	}

	std::array<T, N> mValueArr;

protected:
	virtual bool SetValue(const void* data, size_t size)
	{
		if (size != sizeof(T) * N)
			return false;

		auto& inArr = *(const std::array<T, N>*)data;
		for (size_t i = 0; i < N; i++)
		{
			mValueArr[i] = inArr[i];
		}
		return true;
	}
	virtual bool GetValue(void* data, size_t size)
	{
		if (size != sizeof(T) * N)
			return false;
		auto& outArr = *(std::array<T, N>*)data;
		for (size_t i = 0; i < N; i++)
		{
			outArr[i] = mValueArr[i];
		}
		return true;
	}
};


}