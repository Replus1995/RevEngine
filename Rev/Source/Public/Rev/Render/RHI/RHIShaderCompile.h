#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RHI/RHIShader.h"
#include <map>

namespace Rev
{

enum EShaderCompileMacro : uint64
{
    SCM_NONE = 0,
    SCM_USE_BASECOLOR_TEX = 1,
    SCM_USE_METALLICROUGHNESS_TEX = 1 << 1,
    SCM_USE_NORMAL_TEX = 1 << 2,
    SCM_USE_OCCLUSION_TEX = 1 << 3,
    SCM_USE_EMISSIVE_TEX = 1 << 4
};

struct REV_API FRHIShaderCompileOptions
{
public:
    uint64 mMacros = 0;

    FRHIShaderCompileOptions() = default;
    ~FRHIShaderCompileOptions() = default;

    uint64 Hash() const { return mMacros; };
    void AddMacro(EShaderCompileMacro InMacro)
    {
        mMacros |= (uint64)InMacro;
    }
    void RemoveMacro(EShaderCompileMacro InMacro)
    {
        mMacros &= ~((uint64)InMacro);
    }
};


struct REV_API FCompiledShaders
{
public:
    FCompiledShaders() = default;
    ~FCompiledShaders() = default;
    inline void Add(uint64 OptionHash, const Ref<FRHIShader>& InShader)
    {
        mOptionShaderMap.emplace(OptionHash, InShader);
    }
    inline void Remove(uint64 OptionHash)
    {
        mOptionShaderMap.erase(OptionHash);
    }
    Ref<FRHIShader> operator[](uint64 OptionHash) const
    {
        if (auto iter = mOptionShaderMap.find(OptionHash); iter != mOptionShaderMap.end())
        {
            return iter->second;
        }
        return nullptr;
    }
private:
    std::map<uint64, Ref<FRHIShader>> mOptionShaderMap;
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