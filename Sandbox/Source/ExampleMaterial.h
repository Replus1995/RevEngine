#pragma once
#include <Rev/Render/Material/Material.h>
#include <Rev/Math/Maths.h>
#include <Rev/Render/RHI/RHIShaderLibrary.h>

class ExampleMaterial : public Rev::FMaterial
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
        ShaderProgram = Rev::FRHIShaderLibrary::GetInstance().CreateGraphicsProgram("ExampleProgram", "/Game/Shaders/ExampleVS", "/Game/Shaders/ExamplePS");
    }

    virtual void PreDraw(Rev::FRHICommandList& RHICmdList) override
    {
        Rev::FMaterial::PreDraw(RHICmdList);
        //uColor.Upload(mProgram);
    }


protected:
    //Rev::TMaterialUniform<Rev::Math::FLinearColor> uColor = { "Color", Rev::Math::FLinearColor(1) };
};