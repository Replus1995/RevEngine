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

class REV_API Shader
{
public:
    virtual ~Shader() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual ShaderUniformLocation GetUniformLocation(std::string_view name) = 0;

    virtual void SetUniform(ShaderUniformLocation location, int value) = 0;
    virtual void SetUniform(ShaderUniformLocation location, float value) = 0;
    virtual void SetUniform(ShaderUniformLocation location, const Math::FVector2& value) = 0;
    virtual void SetUniform(ShaderUniformLocation location, const Math::FVector3& value) = 0;
    virtual void SetUniform(ShaderUniformLocation location, const Math::FVector4& value) = 0;
    virtual void SetUniform(ShaderUniformLocation location, const Math::FMatrix3& value) = 0;
    virtual void SetUniform(ShaderUniformLocation location, const Math::FMatrix4& value) = 0;

    template<typename T>
    void SetUniform(const std::string& name, const T& value)
    {
        this->SetUniform(GetUniformLocation(name), value);
    }

    virtual void SetUniformArray(ShaderUniformLocation location, int* values, uint32_t count) = 0;

    template<typename T>
    void SetUniformArray(const std::string& name, const T& value)
    {
        this->SetUniformArray(GetUniformLocation(name), value);
    }


    virtual const std::string& GetName() const = 0;

    static Ref<Shader> Create(const std::string& filepath);
    static Ref<Shader> Create(const std::string& name, const std::string& vertSrc, const std::string& fragSrc);
};

class REV_API ShaderLibrary
{
public:
    void Add(const std::string& name, const Ref<Shader>& shader);
    void Add(const Ref<Shader>& shader);
    Ref<Shader> Load(const std::string& filepath);
    Ref<Shader> Load(const std::string& name, const std::string& filepath);

    Ref<Shader> Get(const std::string& name);

    bool Exists(const std::string& name) const;
private:
    std::unordered_map<std::string, Ref<Shader>> mShaderCache;
};

}