#include "FileSystem.h"
#include "Rev/Core/Log.h"
#include <filesystem>


namespace Rev
{

void FileSystem::MountDir(const std::string& virtualDir, const std::string& nativeDir)
{
	auto res = GetDirMap().emplace(virtualDir, nativeDir);
	if (!res.second)
	{
		RE_CORE_ERROR("VirtualDir \"{0}\" has already been mounted as \"{1}\"", virtualDir.c_str(), res.first->second.c_str());
	}
}

std::string FileSystem::ToNative(const std::string& strpath)
{
	auto& dirMap = GetDirMap();
	for (auto iter = dirMap.begin(); iter != dirMap.end(); iter++)
	{
		if (strpath._Starts_with(iter->first))
		{
			return  iter->second + strpath.substr(iter->first.size());
		}
	}
	return strpath;
}

std::string FileSystem::ToVirtual(const std::string& strpath)
{
	auto& dirMap = GetDirMap();
	for (auto iter = dirMap.begin(); iter != dirMap.end(); iter++)
	{
		if (strpath._Starts_with(iter->second))
		{
			return  iter->first + strpath.substr(iter->second.size());
		}
	}
	return strpath;
}

std::map<std::string, std::string>& FileSystem::GetDirMap()
{
	static std::map<std::string, std::string> sDirMap;
	return sDirMap;
}

Path::Path(const std::string& path)
{
	InnerSetPath(path);
}

Path::~Path()
{
}

std::string Path::ToNative()
{
	return FileSystem::ToNative(mPathStr);
}

Path& Path::operator=(const std::string& path)
{
	InnerSetPath(path);
	return *this;
}

const std::string& Path::operator()() const
{
	return mPathStr;
}

void Path::InnerSetPath(const std::string& path)
{
	std::filesystem::path tPath(path);
	mPathStr = tPath.lexically_normal().generic_u8string();
}

}
