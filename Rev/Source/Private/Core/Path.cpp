#include "Rev/Core/Path.h"
#include "Rev/Core/FileSystem.h"
#include <filesystem>

namespace Rev
{

struct FPathImpl
{
	std::string Name;
	std::string Directory;
	std::string Extension;

	FPathImpl() = default;
	FPathImpl(const char* InFullPath)
	{
		std::filesystem::path tPath(InFullPath);
		if (!tPath.empty())
		{
			tPath.lexically_normal();
			Directory = tPath.parent_path().generic_u8string();
			Name = tPath.stem().generic_u8string();
			Extension = tPath.extension().generic_u8string();
		}
	}
};

FPath::FPath()
	: mImpl(CreateScope<FPathImpl>())
{
}

FPath::FPath(const char* InPath)
	: mImpl(CreateScope<FPathImpl>(InPath))
{
}

FPath::FPath(const std::string& InPath)
	: mImpl(CreateScope<FPathImpl>(InPath.c_str()))
{
}

FPath::FPath(const FPath& Other)
{
	*this = Other;
}

FPath::FPath(FPath&& Other) noexcept
{
	*this = std::move(Other);
}

FPath::~FPath()
{
}

FPath& FPath::operator=(const FPath& Other)
{
	mImpl->Name = Other.mImpl->Name;
	mImpl->Directory = Other.mImpl->Directory;
	mImpl->Extension = Other.mImpl->Extension;
	return *this;
}

FPath& FPath::operator=(FPath&& Other) noexcept
{
	mImpl = std::move(Other.mImpl);
	return *this;
}

bool FPath::Empty() const
{
	return mImpl->Directory.empty() && mImpl->Name.empty();
}

std::string FPath::ToString(bool bHasExt) const
{
	if (bHasExt)
	{
		return mImpl->Directory + "/" + mImpl->Name + mImpl->Extension;
	}
	return mImpl->Directory + "/" + mImpl->Name;
}

const std::string& FPath::Name() const
{
	return mImpl->Name;
}

const std::string& FPath::Directory() const
{
	return mImpl->Directory;
}

const std::string& FPath::Extension() const
{
	return mImpl->Extension;
}

std::string FPath::ToNative() const
{
	return FFileSystem::ToNative(mImpl->Directory) + "/" + mImpl->Name + mImpl->Extension;
}

}