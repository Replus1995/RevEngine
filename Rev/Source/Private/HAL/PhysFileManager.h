#pragma once
#include "Rev/HAL/FIleManager.h"

struct PHYSFS_File;
struct PHYSFS_Stat;

namespace Rev
{

class FPhysFile : public IFile
{
public:
	FPhysFile(PHYSFS_File* InHandle, PHYSFS_Stat* InStat);
	virtual ~FPhysFile() = default;

	virtual void Read(void* Data, int64 Length) override;
	virtual void Write(const void* Data, int64 Length) override;
	virtual void Seek(int64 Pos) override;
	virtual int64 Tell() override;

	virtual int64 GetFileSize() const override { return FileSize; }
	virtual int64 GetLastModTime() const override { return ModTime; }
	virtual int64 GetCreateTime() const override { return CreateTime; }

private:
	friend class FPhysFileManager;

	PHYSFS_File* Handle = nullptr;
	int64 FileSize = 0;
	int64 ModTime = 0;
	int64 CreateTime = 0;
	int64 AccessTime = 0;
};

class FPhysFileManager : public IFileManager
{
public:
	static void Init();
	static void Cleanup();

	FPhysFileManager() {}
	virtual ~FPhysFileManager() = default;

	//Mount APIs
	virtual void Mount(const char* VirtualDir, const char* NativeDir) override;
	virtual void Unmount(const char* VirtualDir) override;
	virtual std::string GetNative(const char* InName) const override;

	//File APIs
	virtual bool IsExist(const char* InName) override;
	virtual bool IsDirectory(const char* InName) override;

	virtual IFile* OpenFile(const char* InName, EFileOpenFlags InOpenFlags) override;
	virtual void CloseFile(IFile*& InFile) override;

	virtual bool LoadBinaryFile(const char* InName, FBuffer& OutBinary) override;
	virtual bool SaveBinaryFile(const char* InName, const FBuffer& InBinary) override;

	virtual bool LoadStringFile(const char* InName, std::string& OutString) override;
	virtual bool SaveStringFile(const char* InName, const std::string& InString) override;
};

}