#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Core/Buffer.h"
#include <string_view>
#include <map>

namespace Rev
{

class FPath
{
public:
	REV_API FPath();
	REV_API FPath(const std::string& InPath);
	REV_API ~FPath();
	REV_API FPath& operator=(const FPath& InPath);

	REV_API bool Empty() const;
	REV_API std::string FullPath(bool WithExt = true) const;
	REV_API const std::string& Name() const;
	REV_API const std::string& Extension() const;
	REV_API std::string ToNative() const;
private:
	std::string mDir;
	std::string mName;
	std::string mExtension;
};

class REV_API FFileSystem
{
public:
	static void MountDir(const std::string& VirtualDir, const std::string& NativeDir);

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