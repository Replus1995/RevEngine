#include "pinepch.h"
#include "FileSystem.h"

namespace Pine
{

void FileSystem::Init()
{
}

std::string FileSystem::ToAbsolutePath(std::string_view strpath)
{
	return std::string();
}

std::string FileSystem::ToProtocolPath(std::string_view strpath)
{
	return std::string();
}

std::map<std::string, std::string>& FileSystem::GetProtocolMap()
{
	static std::map<std::string, std::string> sProtocolMap;
	return sProtocolMap;
}

void FileSystem::AddProtocol(std::string&& protocol, std::string&& abspath)
{
	GetProtocolMap().emplace(std::move(protocol), std::move(abspath));
}

void Path::InnerSetPath(std::string_view path)
{
	mStrPath = FileSystem::ToProtocolPath(path);
}

}
