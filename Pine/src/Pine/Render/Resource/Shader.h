#pragma once
#include "Pine/Core/Base.h"
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

namespace Pine
{
class ShaderUniformLocation
{
public:
    ShaderUniformLocation() : mLocation(0) {};
    ShaderUniformLocation(uint16 location) : mLocation(location) {};
    ~ShaderUniformLocation() = default;

    operator uint16() { return mLocation; }
private:
    uint16 mLocation;
};

class Shader
{
public:
    virtual ~Shader() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual ShaderUniformLocation GetUniformLocation(std::string_view name) = 0;

    virtual void SetUniform(ShaderUniformLocation location, int value) = 0;
    virtual void SetUniform(ShaderUniformLocation location, float value) = 0;
    virtual void SetUniform(ShaderUniformLocation location, const glm::vec2& value) = 0;
    virtual void SetUniform(ShaderUniformLocation location, const glm::vec3& value) = 0;
    virtual void SetUniform(ShaderUniformLocation location, const glm::vec4& value) = 0;
    virtual void SetUniform(ShaderUniformLocation location, const glm::mat3& value) = 0;
    virtual void SetUniform(ShaderUniformLocation location, const glm::mat4& value) = 0;

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

class ShaderLibrary
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