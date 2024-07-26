#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Core/Path.h"
#include "Rev/Core/File.h"
#include "Rev/Core/Buffer.h"
#include <map>

namespace Rev
{

class REV_API FFileSystem
{
public:
	static void Init(const char* argv0);
	static void Shutdown();

	static void MountDir(const std::string& VirtualDir, const std::string& NativeDir);
	static bool Exists(const FPath& InPath);

	static FBuffer LoadBinaryFile(const std::string& InNativePath);
	static FBuffer LoadBinaryFile(const FPath& InPath);
	static bool SaveBinaryFile(const std::string& InNativePath, const FBuffer& InBinary);
	static bool SaveBinaryFile(const FPath& InPath, const FBuffer& InBinary);

	static std::string LoadStringFile(const FPath& InPath);
	static bool SaveStringFile(const FPath& InPath, const std::string& InString);

private:
	friend class FPath;
	static std::string ToNative(const std::string& strpath);
	static std::string ToVirtual(const std::string& strpath);

	static std::map<std::string, std::string>& GetDirMap();
};



}