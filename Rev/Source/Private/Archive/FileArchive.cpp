#include "Rev/Archive/FileArchive.h"
#include "Rev/Core/Log.h"
#include "Rev/HAL/FIleManager.h"

namespace Rev
{

FFileArchive::FFileArchive(const char* InPath, EFileArchiveKind InKind)
	: mKind(InKind)
{
	EFileOpenFlags Flags = InKind == EFileArchiveKind::Read ? FO_Read : FO_Write;
	mFile = IFileManager::Get().OpenFile(InPath, Flags);
	if (!mFile)
	{
		REV_CORE_WARN("FileArchive open failed: {0}", InPath);
	}
}

FFileArchive::~FFileArchive()
{
	IFileManager::Get().CloseFile(mFile);
}

bool FFileArchive::IsLoading() const
{
	return mKind == EFileArchiveKind::Read;
}

void FFileArchive::Serialize(void* pValue, int64 Length)
{
	if(!mFile)
		return;

	switch (mKind)
	{
	case Rev::EFileArchiveKind::Read:
		mFile->Read(pValue, Length);
		break;
	case Rev::EFileArchiveKind::Write:
		mFile->Write(pValue, Length);
		break;
	}
}


}


