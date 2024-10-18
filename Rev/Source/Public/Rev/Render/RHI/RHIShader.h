#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"
#include "Rev/Render/RHI/RHIResource.h"
#include "Rev/Render/RHI/RHIPipeline.h"
#include "Rev/Render/RHI/RHITexture.h"
#include <unordered_map>

namespace Rev
{

enum class ERHIUniformType : uint8
{
    Sampler,
    Image,
    Buffer,
};

struct FRHIUniformInfo
{
    std::string Name;
    ERHIUniformType Type;
    uint8 Num;
    uint16 Binding;

    EPixelFormat TexFormat;
    ETextureDimension TexDimension;
};

enum class ERHIShaderStage : uint8
{
    Unknown = 0,
    Vertex = 1,
    Pixel = 2,
    Hull = 3, //unsupported
    Domain = 4, //unsupported
    Geometry = 5, //unsupported
    Compute = 6, //unsupported
    Count = 7
};

class REV_API FRHIShader : public FRHIResource
{
public:
    virtual ~FRHIShader() = default;
    ERHIShaderStage GetStage() const { return mStage; }

protected:
    FRHIShader(ERHIShaderStage InStage) : mStage(InStage) {}
protected:
    ERHIShaderStage mStage;
};

class FRHITexture;
class REV_API FRHIShaderProgram : public FRHIResource
{
public:
    virtual ~FRHIShaderProgram() = default;

    virtual uint16 GetUniformLocation(std::string_view name) = 0;

    virtual void SetUniform(uint16 location, int value) = 0;
    virtual void SetUniform(uint16 location, float value) = 0;
    virtual void SetUniform(uint16 location, const Math::FVector2& value) = 0;
    virtual void SetUniform(uint16 location, const Math::FVector3& value) = 0;
    virtual void SetUniform(uint16 location, const Math::FVector4& value) = 0;
    virtual void SetUniform(uint16 location, const Math::FMatrix3& value) = 0;
    virtual void SetUniform(uint16 location, const Math::FMatrix4& value) = 0;

    template<typename T>
    void SetUniform(const std::string& name, const T& value)
    {
        this->SetUniform(GetUniformLocation(name), value);
    }

    virtual void SetUniformArray(uint16 location, const int* values, uint32_t count) = 0;

    template<typename T>
    void SetUniformArray(const std::string& name, const T& value)
    {
        this->SetUniformArray(GetUniformLocation(name), value);
    }

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

    Ref<FRHIShader>& operator[](ERHIShaderStage Stage);
    const Ref<FRHIShader>& operator[](ERHIShaderStage Stage) const;
};

}