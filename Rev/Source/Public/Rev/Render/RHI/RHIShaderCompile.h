#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RHI/RHIShader.h"
#include <map>

namespace Rev
{

struct REV_API FRHIShaderCompileOptions
{
public:
    std::string Macros;

    FRHIShaderCompileOptions() = default;
    ~FRHIShaderCompileOptions() = default;

    void AddMacro(const std::string& InMacro);
    uint32 GetHash() const;
};


struct REV_API FCompiledShaders
{
public:
    FCompiledShaders() = default;
    ~FCompiledShaders() = default;
    inline void Add(uint32 OptionHash, const Ref<FRHIShader>& InShader)
    {
        mOptionShaderMap.emplace(OptionHash, InShader);
    }
    inline void Remove(uint32 OptionHash)
    {
        mOptionShaderMap.erase(OptionHash);
    }
    Ref<FRHIShader> operator[](uint32 OptionHash) const
    {
        if (auto iter = mOptionShaderMap.find(OptionHash); iter != mOptionShaderMap.end())
        {
            return iter->second;
        }
        return nullptr;
    }
private:
    std::map<uint32, Ref<FRHIShader>> mOptionShaderMap;
};

struct FRHIShaderCreateDesc
{
    std::string Name; //eg. /Engine/Shaders/Basic
    FRHIShaderCompileOptions Options;

    FRHIShaderCreateDesc() = default;
    FRHIShaderCreateDesc(const char* InName, const FRHIShaderCompileOptions& InOptions = {})
        : Name(InName)
        , Options(InOptions)
    {}
    FRHIShaderCreateDesc(const std::string& InName, const FRHIShaderCompileOptions& InOptions = {})
        : Name(InName)
        , Options(InOptions)
    {}
};


}