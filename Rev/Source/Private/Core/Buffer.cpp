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
	Release();
	mData = new uint8[size];
	mSize = size;
}

void FBuffer::Release()
{
	delete[] mData;
	mData = nullptr;
	mSize = 0;
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
	RE_CORE_ASSERT(mOffset + mSize <= InBuffer.Size());
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