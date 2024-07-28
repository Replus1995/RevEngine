#pragma once
#include "Rev/Core/Base.h"

namespace Rev
{

class REV_API FPath
{
public:
	FPath();
	FPath(const char* InPath);
	FPath(const std::string& InPath);
	FPath(const FPath& Other);
	FPath(FPath&& Other) noexcept;
	~FPath();

	FPath& operator=(const FPath& Other);
	FPath& operator=(FPath&& Other) noexcept;

	bool Empty() const;
	const std::string& Name() const;
	const std::string& Directory() const;
	const std::string& Extension() const;

	std::string ToString(bool bHasExt = true) const;
	std::string ToNative() const;
private:
	Scope<struct FPathImpl> mImpl;
};

}