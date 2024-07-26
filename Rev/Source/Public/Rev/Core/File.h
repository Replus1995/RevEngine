#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Core/Path.h"

namespace Rev
{

enum class EFileOpenOp : uint8
{
	In = 0,
	Out = 1,
	InOut = 3
};

class REV_API FFile
{
public:
	FFile();
	FFile(const FPath& InPath, EFileOpenOp InOp);
	~FFile();

	void Open(const FPath& InPath, EFileOpenOp InOp);
	void Close();
	bool IsOpened() const;

	void Read(void* Data, int64 Length);
	void Write(const void* Data, int64 Length);

	void Seek(int64 Pos, bool bReverse = false);
	int64 Tell();

private:
	Scope<struct FFileImpl> mImpl;
};

}