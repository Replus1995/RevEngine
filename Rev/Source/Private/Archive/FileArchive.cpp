#include "Rev/Archive/FileArchive.h"
#include "Rev/Core/Log.h"

namespace Rev
{

FFileArchive::FFileArchive(const FPath& InPath, EFileArchiveKind InKind)
	: mKind(InKind)
{
	EFileOpenOp FOp = InKind == EFileArchiveKind::Read ? EFileOpenOp::In : EFileOpenOp::Out;
	mFile.Open(InPath, FOp);
	if (!mFile.IsOpened())
	{
		RE_CORE_WARN("FileArchive open failed: {0}", InPath.ToString().c_str());
	}
}

FFileArchive::~FFileArchive()
{
	mFile.Close();
}

bool FFileArchive::IsLoading() const
{
	return mKind == EFileArchiveKind::Read;
}

void FFileArchive::Serialize(void* pValue, int64 Length)
{
	switch (mKind)
	{
	case Rev::EFileArchiveKind::Read:
		mFile.Read(pValue, Length);
		break;
	case Rev::EFileArchiveKind::Write:
		mFile.Write(pValue, Length);
		break;
	}
}


}


