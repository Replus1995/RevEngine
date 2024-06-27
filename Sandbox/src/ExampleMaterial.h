#include <Pine/Render/Material.h>
#include <Pine/Render/Renderer.h>
#include <Pine/Math/Math.h>

class ExampleMaterial : public Pine::Material
{
public:
    ExampleMaterial()
        : Pine::Material(Pine::Renderer::GetShaderLibrary()->Get("ExampleShader"))
    {}
    virtual ~ExampleMaterial() {};

    void SetColor(const Pine::FVector4& color)
    {
        uColor = color;
    }

protected:
    virtual void UploadUniform() override
    {
        Pine::Material::UploadUniform();
        uColor.Upload(mProgram);
    }

protected:
    Pine::TMaterialUniform<Pine::FVector4> uColor = { "Color", Pine::FVector4(1) };
};