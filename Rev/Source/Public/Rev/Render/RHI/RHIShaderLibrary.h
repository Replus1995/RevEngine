#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Core/FileSystem.h"
#include "Rev/Render/RHI/RHIShader.h"

namespace Rev
{

class REV_API FRHIShaderLibrary
{
public:
    static FRHIShaderLibrary& GetInstance();
    void CompileShaderSource(const FPath& InPath, const std::string& InName = "");
    Ref<FRHIShader> FindShader(const std::string& InName, ERHIShaderStage InStage);

    Ref<FRHIShaderProgram> CreateGraphicsProgram(const std::string& InShadersName);
    Ref<FRHIShaderProgram> CreateGraphicsProgram(
        const std::string& InProgramName,
        const std::string& InVertexName,
        const std::string& InPixelName,
        const std::string& InHullName = "",
        const std::string& InDomainName = "",
        const std::string& InGeometryName = ""
    );
    Ref<FRHIShaderProgram> FindProgram(const std::string& InName);

    void ClearShadersCache();
    void ClearShaderProgramsCache();

private:
    FRHIShaderLibrary() = default;
    ~FRHIShaderLibrary() = default;
    friend class RenderCmd;
    static void CreateInstance();
    static void ReleaseInstance();

private:
    std::unordered_map<std::string, FRHIGraphicsShaders> mGraphicsShaderCache;
    std::unordered_map<std::string, Ref<FRHIShaderProgram>> mShaderProgramCache;
};


}