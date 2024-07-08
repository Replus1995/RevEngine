#include "Rev/Core/FileSystem.h"
#include "Rev/Core/Log.h"
#include <filesystem>
#include <iostream>

namespace Rev
{

void FFileSystem::MountDir(const std::string& VirtualDir, const std::string& NativeDir)
{
	auto res = GetDirMap().emplace(VirtualDir, NativeDir);
	if (!res.second)
	{
		RE_CORE_ERROR("VirtualDir \"{0}\" has already been mounted as \"{1}\"", VirtualDir.c_str(), res.first->second.c_str());
	}
}

FBuffer FFileSystem::LoadBinaryFile(const std::string& InNativePath)
{
	FBuffer OutBuffer;
	std::ifstream InFile(InNativePath, std::ios::in | std::ios::binary);
	if (InFile.is_open())
	{
		InFile.seekg(0, std::ios::end);
		auto ContentSize = InFile.tellg();
		InFile.seekg(0, std::ios::beg);

		if (ContentSize > 0)
		{
			OutBuffer.Allocate(ContentSize);
			InFile.read(OutBuffer.DataAs<char>(), ContentSize);
		}
	}
	else
	{
		RE_CORE_ERROR("[FFileSystem] Open file failded '{0}'", InNativePath.c_str());
	}
	return OutBuffer;
}

FBuffer FFileSystem::LoadBinaryFile(const FPath& InPath)
{
	return LoadBinaryFile(InPath.ToNative());
}

bool FFileSystem::SaveBinaryFile(const std::string& InNativePath, const FBuffer& InBinary)
{
	std::ofstream OutFile(InNativePath, std::ios::out | std::ios::binary);
	if (OutFile.is_open())
	{
		if (!InBinary.Empty())
		{
			OutFile.write(InBinary.DataAs<char>(), InBinary.Size());
		}
		return true;
	}
	else
	{
		RE_CORE_ERROR("[FFileSystem] Open file failded '{0}'", InNativePath.c_str());
	}
	return false;
}

bool FFileSystem::SaveBinaryFile(const FPath& InPath, const FBuffer& InBinary)
{
	return SaveBinaryFile(InPath.ToNative(), InBinary);
}

std::string FFileSystem::LoadStringFile(const FPath& InPath)
{
	std::string Result;
	std::ifstream InFile(InPath.ToNative(), std::ios::in | std::ios::binary);
	if (InFile.is_open())
	{
		InFile.seekg(0, std::ios::end);
		auto ContentSize = InFile.tellg();
		InFile.seekg(0, std::ios::beg);

		if (ContentSize > 0)
		{
			Result.resize(ContentSize);
			InFile.read(Result.data(), ContentSize);
		}
	}
	else
	{
		RE_CORE_ERROR("[FFileSystem] Open file failded '{0}'", InPath.String().c_str());
	}
	return Result;
}

bool FFileSystem::SaveStringFile(const FPath& InPath, const std::string& InString)
{
	std::ofstream OutFile(InPath.ToNative(), std::ios::out | std::ios::binary);
	if (OutFile.is_open())
	{
		if (!InString.empty())
		{
			OutFile.write(InString.data(), InString.size());
		}
		return true;
	}
	else
	{
		RE_CORE_ERROR("[FFileSystem] Open file failded '{0}'", InPath.String().c_str());
	}
	return false;
}

std::string FFileSystem::ToNative(const std::string& strpath)
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

std::string FFileSystem::ToVirtual(const std::string& strpath)
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

std::map<std::string, std::string>& FFileSystem::GetDirMap()
{
	static std::map<std::string, std::string> sDirMap;
	return sDirMap;
}

FPath::FPath()
{
}

FPath::FPath(const std::string& InPath)
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

FPath::~FPath()
{
}

FPath& FPath::operator=(const FPath& InPath)
{
	mFullPath = InPath.mFullPath;
	mFileName = InPath.mFileName;
	mFileExtension = InPath.mFileExtension;
	return *this;
}

const std::string& FPath::String() const
{
	return mFullPath;
}

const std::string& FPath::Name() const
{
	return mFileName;
}

const std::string& FPath::Extension() const
{
	return mFileExtension;
}

bool FPath::Empty() const
{
	return mFullPath.empty();
}

std::string FPath::ToNative() const
{
	return FFileSystem::ToNative(mFullPath);
}

}
