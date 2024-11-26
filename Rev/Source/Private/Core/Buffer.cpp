#include "Rev/Core/Buffer.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FBuffer::FBuffer()
{
}

FBuffer::FBuffer(uint64 size)
{
	Allocate(size);
}

FBuffer::~FBuffer()
{
	Release();
}

FBuffer::FBuffer(FBuffer&& other) noexcept
{
	mSize = other.mSize;
	mData = other.mData;
	other.mSize = 0;
	other.mData = nullptr;
}

FBuffer& FBuffer::operator=(FBuffer&& other) noexcept
{
	Release();
	mSize = other.mSize;
	mData = other.mData;
	other.mSize = 0;
	other.mData = nullptr;
	return *this;
}

FBuffer::operator bool() const
{
	return mData != nullptr;
}

FBuffer FBuffer::Copy(FBuffer other)
{
	FBuffer result(other.mSize);
	memcpy(result.mData, other.mData, other.mSize);
	return result;
}

void FBuffer::Allocate(uint64 size)
{
	if (mSize == size)
		return;
	if (void* NewData = realloc(mData, size); NewData)
	{
		mData = (uint8*)NewData;
		mSize = size;
		memset(mData, 0, mSize);
	}
}

void FBuffer::Release()
{
	free(mData);
	mData = nullptr;
	mSize = 0;
}

FArchive& operator<<(FArchive& Ar, FBuffer& Buffer)
{
	uint64 BufferSize = 0;
	if (Ar.IsLoading())
	{
		Ar << BufferSize;
		Buffer.Allocate(BufferSize);
		Ar.Serialize(Buffer.Data(), BufferSize);
	}
	else
	{
		BufferSize = Buffer.Size();
		Ar << BufferSize;
		Ar.Serialize(Buffer.Data(), BufferSize);
	}
	return Ar;
}

FBufferView::FBufferView()
	: mData(nullptr)
	, mOffset(0)
	, mSize(0)
{
}

FBufferView::FBufferView(const FBuffer& InBuffer, uint64 InOffset, uint64 InSize)
	: mData(InBuffer.mData)
	, mOffset(InOffset)
	, mSize(InSize)
{
	REV_CORE_ASSERT(mOffset + mSize <= InBuffer.Size());
}

FBufferView::~FBufferView()
{
}

FBufferView& FBufferView::operator=(const FBufferView& Other)
{
	mData = Other.mData;
	mOffset = Other.mOffset;
	mSize = Other.mSize;
	return *this;
}

}
