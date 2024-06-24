#pragma once
#include "Pine/Core/Base.h"
#include "Pine/Core/Assert.h"
#include "Pine/Render/Resource/Shader.h"

namespace Pine
{

class MaterialUniform
{
public:
	MaterialUniform(std::string_view name)
		: mName(name)
	{}
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

	virtual void Upload(const Ref<Shader>& program) = 0;

protected:
	virtual bool SetValue(const void* data, size_t size) = 0;
	virtual bool GetValue(void* data, size_t size) = 0;

	std::string mName;
	ShaderUniformLocation mLocation;
};

template<typename T>
class TMaterialUniform : public MaterialUniform
{
public:
	TMaterialUniform(std::string_view name, const T& value)
		: MaterialUniform(name)
		, mValue(value)
	{}

	TMaterialUniform(std::string_view name, ShaderUniformLocation location, const T& value)
		: MaterialUniform(name)
		, mValue(value)
	{
		mLocation = location;
	}

	virtual void Upload(const Ref<Shader>& program)
	{
		PE_CORE_ASSERT(program);
		if(!mLocation.IsValid())
			mLocation = program->GetUniformLocation(mName);
		program->SetUniform(mLocation, mValue);
	}

	TMaterialUniform<T>& operator=(const T& value)
	{
		mValue = value;
		return *this;
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
	TMaterialUniform(std::string_view name, const std::array<T, N>& value)
		: MaterialUniform(name)
		, mValue(value)
	{
	}

	TMaterialUniform(std::string_view name, ShaderUniformLocation location, const std::array<T, N>& value)
		: MaterialUniform(name)
		, mValue(value)
	{
		mLocation = location;
	}

	virtual void Upload(const Ref<Shader>& program)
	{
		PE_CORE_ASSERT(program);
		if (!mLocation.IsValid())
			mLocation = program->GetUniformLocation(mName);
		program->SetUniformArray(mLocation, mValueArr.data(), N);
	}

	TMaterialUniform<std::array<T, N>>& operator=(const std::array<T, N>& value)
	{
		mValue = value;
		return *this;
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