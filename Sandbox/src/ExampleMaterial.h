#include <Rev/Render/Material/Material.h>
#include <Rev/Render/Renderer.h>
#include <Rev/Math/Maths.h>
#include <Rev/Render/RHI/RHIShaderLibrary.h>

class ExampleMaterial : public Rev::Material
{
public:
    ExampleMaterial()
        : Rev::Material(Rev::FRHIShaderLibrary::GetInstance().FindProgram("ExampleProgram"))
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
    Rev::TMaterialUniform<Rev::Math::FLinearColor> uColor = { "Color", Rev::Math::FLinearColor(1) };
};