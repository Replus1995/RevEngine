#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"
#include <string>
#include <unordered_map>

namespace Rev
{
class REV_API ShaderUniformLocation
{
public:
    ShaderUniformLocation() : mLocation(uint16(-1)) {};
    ShaderUniformLocation(uint16 location) : mLocation(location) {};
    ~ShaderUniformLocation() = default;

    operator uint16() { return mLocation; }
    bool IsValid() const { return mLocation != uint16(-1); }

private:
    uint16 mLocation;
};

class FRHITexture;
class REV_API FRHIShader
{
public:
    virtual ~FRHIShader() = default;

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


    virtual const std::string& GetName() const = 0;

    static Ref<FRHIShader> Create(const std::string& filepath);
    static Ref<FRHIShader> Create(const std::string& name, const std::string& vertSrc, const std::string& fragSrc);
};

class REV_API FRHIShaderLibrary
{
public:
    void Add(const std::string& name, const Ref<FRHIShader>& shader);
    void Add(const Ref<FRHIShader>& shader);
    Ref<FRHIShader> Load(const std::string& filepath);
    Ref<FRHIShader> Load(const std::string& name, const std::string& filepath);

    Ref<FRHIShader> Get(const std::string& name);

    bool Exists(const std::string& name) const;
private:
    std::unordered_map<std::string, Ref<FRHIShader>> mShaderCache;
};

}