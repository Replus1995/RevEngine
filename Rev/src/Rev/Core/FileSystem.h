#pragma once
#include "Rev/Core/Base.h"
#include <string_view>
#include <map>

namespace Rev
{

class REV_API FileSystem
{
public:
	static void MountDir(const std::string& virtualDir, const std::string& nativeDir);

private:
	friend class Path;
	static std::string ToNative(const std::string& strpath);
	static std::string ToVirtual(const std::string& strpath);

	static std::map<std::string, std::string>& GetDirMap();
};

class Path
{
public:
	REV_API Path();
	REV_API Path(const std::string& InPath);
	REV_API ~Path();
	REV_API Path& operator=(const Path& InPath);
	
	REV_API const std::string& Name() const;
	REV_API const std::string& Extension() const;
	REV_API bool Empty() const;
	REV_API std::string ToNative();
private:
	std::string mFullPath;
	std::string mFileName;
	std::string mFileExtension;
};



}