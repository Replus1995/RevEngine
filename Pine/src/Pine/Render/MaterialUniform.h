#pragma once
#include "Pine/Core/Base.h"
#include "Pine/Core/Assert.h"
#include "Pine/Render/Resource/Shader.h"

namespace Pine
{

class MaterialUniform
{
public:
	virtual ~MaterialUniform() = default;

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
	MaterialUniform(std::string_view name, Shader* program)
		: mName(name)
	{
		PE_CORE_ASSERT(program)
		mLocation = program->GetUniformLocation(name);
	}
	MaterialUniform(std::string_view name, uint16_t location)
		: mName(name)
		, mLocation(location)
	{
	}

	virtual bool SetValue(const void* data, size_t size) = 0;
	virtual bool GetValue(void* data, size_t size) = 0;

	std::string mName;
	ShaderUniformLocation mLocation;
};

template<typename T>
class TMaterialUniform : public MaterialUniform
{
public:
	TMaterialUniform(std::string_view name, const T& value, Shader* program)
		: MaterialUniform(name, program)
		, mValue(value)
	{
	}

	TMaterialUniform(std::string_view name, const T& value, uint16_t location)
		: MaterialUniform(name, location)
		, mValue(value)
	{
	}

	virtual void Upload(Shader* pProgram)
	{
		pProgram->SetUniform(mName, mValue);
	}

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

private:
	T mValue;
};

template<typename T, size_t N>
class TMaterialUniform<std::array<T, N>> : public MaterialUniform
{
public:
	TMaterialUniform(std::string_view name, const std::array<T, N>& value, Shader* program)
		: MaterialUniform(name, program)
		, mValue(value)
	{
	}

	TMaterialUniform(std::string_view name, const std::array<T, N>& value, uint16_t location)
		: MaterialUniform(name, location)
		, mValue(value)
	{
	}

	virtual void Upload(Shader* pProgram)
	{
		pProgram->SetUniformArray(mName, mValueArr.data(), N);
	}

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

private:
	std::array<T, N> mValueArr;
};


}