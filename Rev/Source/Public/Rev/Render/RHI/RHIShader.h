#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"
#include "Rev/Render/PixelFormat.h"
#include "Rev/Render/RHI/RHIResource.h"
#include "Rev/Render/RHI/RHIDefinitions.h"
#include <unordered_map>

namespace Rev
{

class FRHITexture;

struct FRHIShaderAttribute
{
    std::string Name;
    EVertexElmentType Type = EVertexElmentType::Unknown;
    uint8 StreamIndex = 0;
};

struct FRHIShaderUniform
{
    std::string Name;
    EShaderUniformType Type = EShaderUniformType::Buffer;
    uint8 Num = 1;
    uint16 Binding = 0;

    EPixelFormat TexFormat = PF_Unknown;
    ETextureDimension TexDimension = ETextureDimension::Texture2D;
    int8 SamplerBinding = -1; //Sampler binding should not be larger than 15
    bool bSamplerCompare = false;
};

class REV_API FRHIShader : public FRHIResource
{
public:
    virtual ~FRHIShader() = default;
    EShaderStage GetStage() const { return Stage; }

protected:
    FRHIShader(EShaderStage InStage) : Stage(InStage) {}
protected:
    EShaderStage Stage;
};

class REV_API FRHIShaderProgram : public FRHIResource
{
public:
    virtual ~FRHIShaderProgram() = default;
    const std::string& GetName() const { return mProgramName; };

protected:
    FRHIShaderProgram(const std::string& InName) : mProgramName(InName) {}
    std::string mProgramName;
};


struct REV_API FRHIGraphicsShaders
{
    Ref<FRHIShader> VertexShader = nullptr;
    Ref<FRHIShader> HullShader = nullptr;
    Ref<FRHIShader> DomainShader = nullptr;
    Ref<FRHIShader> PixelShader = nullptr;
    Ref<FRHIShader> GeometryShader = nullptr;

    Ref<FRHIShader>& operator[](uint8 StageIndex);
    const Ref<FRHIShader>& operator[](uint8 StageIndex) const;
};

}