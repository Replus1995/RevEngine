#pragma once
#include "Pine/Core/Base.h"
#include <string_view>
#include <map>

namespace Pine
{

class PINE_API FileSystem
{
public:
	static void MountDir(const std::string& virtualDir, const std::string& nativeDir);

private:
	friend class Path;
	static std::string ToNative(const std::string& strpath);
	static std::string ToVirtual(const std::string& strpath);

	static std::map<std::string, std::string>& GetDirMap();
};

class PINE_API Path
{
public:
	Path(const std::string& path = "");
	~Path();

	std::string ToNative();

	Path& operator=(const std::string& path);
	const std::string& operator()() const;
private:
	void InnerSetPath(const std::string& path);

private:
	std::string mPathStr;
};



}