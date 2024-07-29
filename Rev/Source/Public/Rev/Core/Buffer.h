#pragma once
#include "Rev/Core/Base.h" 
#include "Rev/Archive/Archive.h"
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

	friend FArchive& operator<<(FArchive& Ar, FBuffer& Buffer);

private:
	friend class FBufferView;
	uint8* mData = nullptr;
	uint64 mSize = 0;
};

class REV_API FBufferView
{
public:
	FBufferView();
	FBufferView(const FBuffer& InBuffer, uint64 InOffset, uint64 InSize);
	~FBufferView();

	FBufferView& operator=(const FBufferView& Other);

	inline bool Empty() const { return mSize == 0; }
	inline uint64 Size() const { return mSize; }
	inline uint8* Data() { return mData ? mData + mOffset : nullptr; }
	inline const uint8* Data() const { return  mData ? mData + mOffset : nullptr; }

	template<typename T>
	T* DataAs() { return (T*)Data(); }
	template<typename T>
	const T* DataAs() const { return (const T*)Data(); }

private:
	uint8* mData;
	uint64 mOffset;
	uint64 mSize;
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