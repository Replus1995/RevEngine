#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"
#include "Rev/Render/RHI/RHIResource.h"
#include <string>
#include <unordered_map>

namespace Rev
{

enum class ERHIShaderStage : uint8
{
    Unknown = 0,
    Vertex = 1,
    Hull = 2, //unsupported
    Domain = 3, //unsupported
    Pixel = 4,
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

struct REV_API FRHIGraphicsShaders
{
    Ref<FRHIShader> VertexShader = nullptr;
    Ref<FRHIShader> HullShader = nullptr;
    Ref<FRHIShader> DomainShader = nullptr;
    Ref<FRHIShader> PixelShader = nullptr;
    Ref<FRHIShader> GeometryShader = nullptr;

    const Ref<FRHIShader>& operator[](ERHIShaderStage Stage) const;
};

class REV_API FRHIShaderProgram : public FRHIResource
{
public:
    virtual ~FRHIShaderProgram() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

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
protected:
    std::string mProgramName;
};

class REV_API FRHIShaderUniformProxy
{
public:
    FRHIShaderUniformProxy() : mLocation(uint16(-1)) {}
    FRHIShaderUniformProxy(uint16 InLocation) : mLocation(InLocation) {}
    ~FRHIShaderUniformProxy() {}

    FRHIShaderUniformProxy& operator=(const FRHIShaderUniformProxy& Other)
    {
        mLocation = Other.mLocation;
        return *this;
    }

    template<typename T>
    void UpdateUniform(const Ref<FRHIShaderProgram>& InProgram, const T& InValue)
    {
        if(mLocation != uint16(-1) && InProgram != nullptr)
            InProgram->SetUniform(mLocation, InValue);
    }

    template<typename T>
    void UpdateUniformArray(const Ref<FRHIShaderProgram>& InProgram, const T* InValues, uint32 InCount)
    {
        if (mLocation != uint16(-1) && InProgram != nullptr)
            InProgram->SetUniformArray(mLocation, InValues, InCount);
    }

private:
    uint16 mLocation;
};

}