#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Core/File.h"
#include "Rev/Archive/Archive.h"

namespace Rev
{

enum class EFileArchiveKind
{
	Read,
	Write
};

class FFileArchive : public FArchive
{
public:
	FFileArchive(const FPath& InPath, EFileArchiveKind InKind);
	virtual ~FFileArchive();

	virtual bool IsLoading() const override;
	virtual void Serialize(void* pValue, int64 Length) override;

private:
	EFileArchiveKind mKind;
	FFile mFile;
};

}