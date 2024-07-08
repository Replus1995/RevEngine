#pragma once
#include "Rev/Core/Base.h" 
#include <stdint.h>
#include <cstring>


namespace Rev
{

class REV_API FBuffer
{
public:
	FBuffer();
	FBuffer(uint64 size);
	~FBuffer();

	FBuffer(const FBuffer&) = delete;
	FBuffer(FBuffer&& other) noexcept;

	FBuffer& operator=(const FBuffer&) = delete;
	FBuffer& operator=(FBuffer&& other) noexcept;
	operator bool() const;

	static FBuffer Copy(FBuffer other);

	template<typename T>
	void Allocate(uint64 count) { Allocate(count * sizeof(T)); }
	void Allocate(uint64 size);
	void Release();

	inline bool Empty() const { return mSize == 0; }
	inline uint64 Size() const { return mSize; }
	inline uint8* Data() { return mData; }
	inline const uint8* Data() const { return mData; }

	template<typename T>
	T* DataAs() { return (T*)mData; }
	template<typename T>
	const T* DataAs() const { return (const T*)mData; }

private:
	uint8* mData = nullptr;
	uint64 mSize = 0;
};

template<uint64 N>
class FScopedBuffer
{
	FScopedBuffer()
	{
	}

	~FScopedBuffer()
	{
	}

	uint64 Size() const { return N; }
	uint8* Data() { return mBuffer.mData; }
	const uint8* Data() const { return mData; }

	template<typename T>
	T* As() { return (T*)mData; }
	template<typename T>
	const T* As() const { return (const T*)mData; }

	operator bool() const { return mBuffer; }
private:
	uint8 mData[N];
};

}