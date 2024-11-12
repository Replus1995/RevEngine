#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Core/Buffer.h"

namespace Rev
{

enum EFileOpenFlags : uint8
{
	FO_Read,
	FO_Write,
	FO_Append
};

class REV_API IFile
{
protected:
	IFile() {}

public:
	IFile(const IFile&) = delete;
	virtual ~IFile() = default;

	virtual void Read(void* Data, int64 Length) = 0;
	virtual void Write(const void* Data, int64 Length) = 0;
	virtual void Seek(int64 Pos) = 0;
	virtual int64 Tell() = 0;

	virtual int64 GetFileSize() const = 0;
	virtual int64 GetLastModTime() const = 0;
	virtual int64 GetCreateTime() const = 0;
};


class REV_API IFileManager
{
protected:
	IFileManager() {}

public:
	static IFileManager& Get();
	static void Init();
	static void Cleanup();

	IFileManager(const IFileManager&) = delete;
	virtual ~IFileManager() = default;

	//Mount APIs
	virtual void Mount(const char* VirtualDir, const char* NativeDir) = 0;
	virtual void Unmount(const char* VirtualDir) = 0;
	virtual std::string GetNative(const char* InName) const = 0;

	//File APIs
	virtual bool IsExist(const char* InName) = 0;
	virtual bool IsDirectory(const char* InName) = 0;

	virtual IFile* OpenFile(const char* InName, EFileOpenFlags InOpenFlags) = 0;
	virtual void CloseFile(IFile*& InFile) = 0;

	virtual bool LoadBinaryFile(const char* InName, FBuffer& OutBinary) = 0;
	virtual bool SaveBinaryFile(const char* InName, const FBuffer& InBinary) = 0;

	virtual bool LoadStringFile(const char* InName, std::string& OutString) = 0;
	virtual bool SaveStringFile(const char* InName, const std::string& InString) = 0;
};




}