#include "PhysFileManager.h"
#include "Rev/Misc/ArgParser.h"
#include "physfs.h"

namespace Rev
{

void LogPhysError()
{
	REV_CORE_ERROR("{}", PHYSFS_getLastError());
}

FPhysFile::FPhysFile(PHYSFS_File* InHandle, PHYSFS_Stat* InStat)
	: Handle(InHandle)
{
	REV_CORE_ASSERT(InStat);
	FileSize = InStat->filesize;
	ModTime = InStat->modtime;
	CreateTime = InStat->createtime;
	AccessTime = InStat->accesstime;
}

void FPhysFile::Read(void* Data, int64 Length)
{
	int64 res = PHYSFS_readBytes(Handle, Data, Length);
	if(res < Length)
		LogPhysError();
}

void FPhysFile::Write(const void* Data, int64 Length)
{
	int64 res = PHYSFS_writeBytes(Handle, Data, Length);
	if (res < Length)
		LogPhysError();
}

void FPhysFile::Seek(int64 Pos)
{
	if(PHYSFS_seek(Handle, Pos) == 0)
		LogPhysError();
}

int64 FPhysFile::Tell()
{
	int64 res = PHYSFS_tell(Handle);
	if (res < 0)
	{
		LogPhysError();
		return 0;
	}
	return res;
}

void FPhysFileManager::Init()
{
	if (PHYSFS_init(GArgParser.GetRawArg(0)) == 0)
		LogPhysError();
}

void FPhysFileManager::Cleanup()
{
	if (PHYSFS_deinit() == 0)
		LogPhysError();
}

void FPhysFileManager::Mount(const char* VirtualDir, const char* NativeDir)
{
	if (PHYSFS_mount(NativeDir, VirtualDir, 1) == 0)
		LogPhysError();
}

void FPhysFileManager::Unmount(const char* VirtualDir)
{
	if (PHYSFS_unmount(VirtualDir) == 0)
		LogPhysError();
}

std::string FPhysFileManager::GetNative(const char* InName) const
{
	if (const char* RealDir = PHYSFS_getRealDir(InName); RealDir != nullptr)
	{
		if (const char* MountPoint = PHYSFS_getMountPoint(RealDir); MountPoint != nullptr)
		{
			size_t MountPointLen = strlen(MountPoint);
			return std::string(RealDir) + std::string(InName + MountPointLen);
		}
	}
	LogPhysError();
	return std::string(InName);
}

bool FPhysFileManager::IsExist(const char* InName)
{
	return PHYSFS_exists(InName);
}

bool FPhysFileManager::IsDirectory(const char* InName)
{
	PHYSFS_Stat FileStat;
	if (PHYSFS_stat(InName, &FileStat) == 0)
	{
		LogPhysError();
		return false;
	}
	return FileStat.filetype = PHYSFS_FILETYPE_DIRECTORY;
}

IFile* FPhysFileManager::OpenFile(const char* InName, EFileOpenFlags InOpenFlags)
{
	PHYSFS_Stat FileStat;
	PHYSFS_File* FileHandle = nullptr;
	
	if (PHYSFS_stat(InName, &FileStat) == 0)
	{
		LogPhysError();
		return nullptr;
	}
	if (FileStat.filetype != PHYSFS_FILETYPE_REGULAR)
	{
		REV_CORE_ERROR("[FPhysFileManager] {} is not a regular file.", InName);
		return nullptr;
	}
	if (FileStat.readonly && InOpenFlags != FO_Read)
	{
		REV_CORE_ERROR("[FPhysFileManager] {} is readonly, can not be written to.", InName);
		return nullptr;
	}

	switch (InOpenFlags)
	{
	case Rev::FO_Read:
		FileHandle = PHYSFS_openRead(InName);
		break;
	case Rev::FO_Write:
		FileHandle = PHYSFS_openWrite(InName);
		break;
	case Rev::FO_Append:
		FileHandle = PHYSFS_openAppend(InName);
		break;
	default:
		break;
	}

	if (FileHandle == nullptr)
	{
		LogPhysError();
		return nullptr;
	}

	return new FPhysFile(FileHandle, &FileStat);
}

void FPhysFileManager::CloseFile(IFile*& InFile)
{
	if(!InFile)
		return;

	FPhysFile* File = static_cast<FPhysFile*>(InFile);
	if(PHYSFS_close(File->Handle) == 0)
		LogPhysError();
	delete File;
	InFile = nullptr;
}

bool FPhysFileManager::LoadBinaryFile(const char* InName, FBuffer& OutBinary)
{
	IFile* File = OpenFile(InName, FO_Read);
	if (!File)
	{
		LogPhysError();
		return false;
	}
	OutBinary.Allocate(File->GetFileSize());
	File->Read(OutBinary.Data(), OutBinary.Size());
	CloseFile(File);
	return true;
}

bool FPhysFileManager::SaveBinaryFile(const char* InName, const FBuffer& InBinary)
{
	IFile* File = OpenFile(InName, FO_Write);
	if (!File)
	{
		LogPhysError();
		return false;
	}
	File->Write(InBinary.Data(), InBinary.Size());
	CloseFile(File);
	return true;
}

bool FPhysFileManager::LoadStringFile(const char* InName, std::string& OutString)
{
	IFile* File = OpenFile(InName, FO_Read);
	if (!File)
	{
		return false;
	}
	OutString.resize(File->GetFileSize());
	File->Read(OutString.data(), OutString.size());
	CloseFile(File);
	return true;
}

bool FPhysFileManager::SaveStringFile(const char* InName, const std::string& InString)
{
	IFile* File = OpenFile(InName, FO_Write);
	if (!File)
	{
		return false;
	}
	File->Write(InString.data(), InString.size());
	CloseFile(File);
	return true;
}





}