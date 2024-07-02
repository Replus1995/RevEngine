#pragma once
#include <stdint.h>
#include <cstring>

namespace Rev
{

struct Buffer
{
	uint8_t* Data = nullptr;
	uint64_t Size = 0;

	Buffer() = default;
	Buffer(uint64_t size)
	{
		Allocate(size);
	}

	Buffer(const Buffer&) = default;

	static Buffer Copy(Buffer other)
	{
		Buffer result(other.Size);
		memcpy(result.Data, other.Data, other.Size);
		return result;
	}

	void Allocate(uint64_t size)
	{
		Release();

		Data = new uint8_t[size];
		Size = size;
	}

	void Release()
	{
		delete[] Data;
		Data = nullptr;
		Size = 0;
	}

	template<typename T>
	T* As()
	{
		return (T*)Data;
	}

	operator bool() const
	{
		return (bool)Data;
	}

};

struct ScopedBuffer
{
	ScopedBuffer(Buffer buffer)
		: mBuffer(buffer)
	{
	}

	ScopedBuffer(uint64_t size)
		: mBuffer(size)
	{
	}

	~ScopedBuffer()
	{
		mBuffer.Release();
	}

	uint8_t* Data() { return mBuffer.Data; }
	uint64_t Size() { return mBuffer.Size; }

	template<typename T>
	T* As()
	{
		return mBuffer.As<T>();
	}

	operator bool() const { return mBuffer; }
private:
	Buffer mBuffer;
};

}