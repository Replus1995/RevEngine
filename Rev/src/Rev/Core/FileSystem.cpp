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

Path::Path()
{
}

Path::Path(const std::string& InPath)
{
	if (!InPath.empty())
	{
		std::filesystem::path tPath(InPath);
		tPath.lexically_normal();
		mFullPath = tPath.generic_u8string();
		mFileName = tPath.stem().generic_u8string();
		mFileExtension = tPath.extension().generic_u8string();
	}
}

Path::~Path()
{
}

Path& Path::operator=(const Path& InPath)
{
	mFullPath = InPath.mFullPath;
	mFileName = InPath.mFileName;
	mFileExtension = InPath.mFileExtension;
	return *this;
}

const std::string& Path::Name() const
{
	return mFileName;
}

const std::string& Path::Extension() const
{
	return mFileExtension;
}

bool Path::Empty() const
{
	return mFullPath.empty();
}

std::string Path::ToNative()
{
	return FileSystem::ToNative(mFullPath);
}

}
