#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Core/FileSystem.h"
#include "Rev/Render/RHI/RHIShader.h"
#include "Rev/Render/RHI/RHIShaderCompile.h"

namespace Rev
{

class REV_API FRHIShaderLibrary
{
public:
    static FRHIShaderLibrary& GetInstance();

    Ref<FRHIShaderProgram> CreateGraphicsProgram(
        const std::string& InProgramName,
        const FRHIShaderCreateDesc& InVertexDesc,
        const FRHIShaderCreateDesc& InFragmentDesc,
        const FRHIShaderCreateDesc& InTessControlDesc = {},
        const FRHIShaderCreateDesc& InTessEvalDesc = {},
        const FRHIShaderCreateDesc& InGeometryDesc = {}
    );

    void ClearShadersCache();
private:
    Ref<FRHIShader> LoadOrCompileShader(const FPath& InPath, const FRHIShaderCompileOptions& InOptions, ERHIShaderStage InStage = ERHIShaderStage::Unknown);
    Ref<FRHIShader> CreateShader(const FRHIShaderCreateDesc& InDesc, ERHIShaderStage InStage = ERHIShaderStage::Unknown);

private:
    FRHIShaderLibrary() = default;
    ~FRHIShaderLibrary() = default;
    friend class RenderCmd;
    static void CreateInstance();
    static void ReleaseInstance();

private:
    std::unordered_map<std::string, FCompiledShaders> mShadersCache;
};


}