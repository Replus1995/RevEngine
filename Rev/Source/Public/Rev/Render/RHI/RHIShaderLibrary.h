#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Core/FileSystem.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/RHI/RHIShader.h"
#include "Rev/Render/RHI/RHIShaderCompile.h"

namespace Rev
{

class REV_API FRHIShaderLibrary
{
public:
    static void Initialize(ERenderAPI InAPI);
    static void Shutdown();
    static inline FRHIShaderLibrary* Get() { return ShaderLibrary; }

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
    Ref<FRHIShader> LoadOrCompileShader(const FPath& InPath, const FRHIShaderCompileOptions& InOptions, EShaderStage InStage = SS_Unknown);
    Ref<FRHIShader> CreateShader(const FRHIShaderCreateDesc& InDesc, EShaderStage InStage = SS_Unknown);

private:
    FRHIShaderLibrary() = default;
    ~FRHIShaderLibrary() = default;

private:
    static FRHIShaderLibrary* ShaderLibrary;
    std::unordered_map<std::string, FCompiledShaders> mShadersCache;
};

struct FShaderCompileConfig
{
    uint32 BufferOffset = 0;
    uint32 SamplerOffset = 0;
    uint32 TextureOffset = 0;
};
extern FShaderCompileConfig GShaderCompileConfig;

}