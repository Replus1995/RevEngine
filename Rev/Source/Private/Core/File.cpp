#include "Rev/Core/File.h"
#include <fstream>

namespace Rev
{

static int InitOpenOptions(EFileOpenOp InOp)
{
	int Ops = std::ios::binary;
	switch (InOp)
	{
	case Rev::EFileOpenOp::In:
		Ops |= std::ios::in;
		break;
	case Rev::EFileOpenOp::Out:
		Ops |= std::ios::out;
		break;
	case Rev::EFileOpenOp::InOut:
		Ops |= std::ios::in | std::ios::out;
		break;
	default:
		break;
	}
	return Ops;
}

struct FFileImpl
{
	std::fstream Stream;
};

FFile::FFile()
	: mImpl(CreateScope<FFileImpl>())
{
}

FFile::FFile(const FPath& InPath, EFileOpenOp InOp)
	: FFile()
{
	Open(InPath, InOp);
}

FFile::~FFile()
{
}

void FFile::Open(const FPath& InPath, EFileOpenOp InOp)
{
	if(IsOpened())
		return;
	mImpl->Stream.open(InPath.ToNative(), InitOpenOptions(InOp));
}

void FFile::Close()
{
	mImpl->Stream.close();
}

bool FFile::IsOpened() const
{
	return mImpl->Stream.is_open();
}

void FFile::Read(void* Data, int64 Length)
{
	mImpl->Stream.read((char*)Data, Length);
}

void FFile::Write(const void* Data, int64 Length)
{
	mImpl->Stream.write((const char*)Data, Length);
}

void FFile::Seek(int64 Pos, bool bReverse)
{
	int SeekDir = bReverse ? std::ios::end : std::ios::beg;
	mImpl->Stream.seekg(Pos, SeekDir);
}

int64 FFile::Tell()
{
	return mImpl->Stream.tellg();
}

}

