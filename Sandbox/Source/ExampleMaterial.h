#pragma once
#include <Rev/Render/Material/SurfaceMaterial.h>
#include <Rev/Math/Maths.h>
#include <Rev/Render/RHI/RHIShaderLibrary.h>

class ExampleMaterial : public Rev::SurfaceMaterial
{
public:
    ExampleMaterial() {}
    virtual ~ExampleMaterial() {}

    void SetColor(const Rev::Math::FVector4& color)
    {
        //uColor = color;
    }

    virtual void Compile() override
    {
        mProgram = Rev::FRHIShaderLibrary::GetInstance().CreateGraphicsProgram("ExampleProgram", "/Game/Shaders/ExampleVS", "/Game/Shaders/ExamplePS");
    }

    virtual void PreDraw() override
    {
        Rev::Material::PreDraw();
        //uColor.Upload(mProgram);
    }


protected:
    //Rev::TMaterialUniform<Rev::Math::FLinearColor> uColor = { "Color", Rev::Math::FLinearColor(1) };
};