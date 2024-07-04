#include <Rev/Render/Material/Material.h>
#include <Rev/Render/Renderer.h>
#include <Rev/Math/Maths.h>

class ExampleMaterial : public Rev::Material
{
public:
    ExampleMaterial()
        : Rev::Material(Rev::Renderer::GetShaderLibrary()->Get("ExampleShader"))
    {}
    virtual ~ExampleMaterial() {};

    void SetColor(const Rev::Math::FVector4& color)
    {
        uColor = color;
    }

protected:
    virtual void UploadUniform() override
    {
        Rev::Material::UploadUniform();
        uColor.Upload(mProgram);
    }

protected:
    Rev::TMaterialUniform<Rev::Math::FVector4> uColor = { "Color", Rev::Math::FVector4(1) };
};