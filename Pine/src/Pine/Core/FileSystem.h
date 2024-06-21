#pragma once
#include "Pine/Core/Base.h"
#include <string_view>
#include <map>

namespace Pine
{

class PINE_API FileSystem
{
public:
	static void Init();

private:
	friend class Path;
	static std::string ToAbsolutePath(std::string_view strpath);
	static std::string ToProtocolPath(std::string_view strpath);

private:
	struct Protocol
	{
		std::string mProtocol;
		std::string mAbsPath;
	};
	static std::map<std::string, std::string>& GetProtocolMap();
	static void AddProtocol(std::string&& protocol, std::string&& abspath);
};

class PINE_API Path
{
public:
	Path() = default;
	Path(std::string_view path)
	{
		InnerSetPath(path);
	}
	~Path() = default;

	Path& operator=(std::string_view path)
	{
		InnerSetPath(path);
		return *this;
	}

	const std::string& operator()()
	{
		return mStrPath;
	}

	std::string ToAbsPath()
	{
		return FileSystem::ToAbsolutePath(mStrPath);
	}

private:
	void InnerSetPath(std::string_view path);
	std::string mStrPath;
};



}