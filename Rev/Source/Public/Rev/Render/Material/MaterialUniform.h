#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Core/Assert.h"
#include "Rev/Render/RHI/RHIShader.h"

namespace Rev
{

class REV_API MaterialUniform
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

	virtual void Upload(const Ref<FRHIShaderProgram>& program) = 0;

protected:
	virtual bool SetValue(const void* data, size_t size) = 0;
	virtual bool GetValue(void* data, size_t size) = 0;
	static std::string GetProgramName(const std::string& name);

	std::string mName;
	uint16 mLocation = uint16(-1);
};

template<typename T>
class TMaterialUniform : public MaterialUniform
{
public:
	TMaterialUniform(std::string_view name, const T& value)
		: MaterialUniform(name)
		, mValue(value)
	{}

	TMaterialUniform(std::string_view name, uint16 location, const T& value)
		: MaterialUniform(name)
		, mValue(value)
	{
		mLocation = location;
	}

	virtual void Upload(const Ref<FRHIShaderProgram>& program)
	{
		RE_CORE_ASSERT(program);
		if(mLocation == uint16(-1))
			mLocation = program->GetUniformLocation(GetProgramName(mName));
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

	TMaterialUniform(std::string_view name, uint16 location, const std::array<T, N>& value)
		: MaterialUniform(name)
		, mValue(value)
	{
		mLocation = location;
	}

	virtual void Upload(const Ref<FRHIShaderProgram>& program)
	{
		PE_CORE_ASSERT(program);
		if (mLocation == uint16(-1))
			mLocation = program->GetUniformLocation(GetProgramName(mName));
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