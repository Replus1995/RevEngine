#include <Pine/Render/Material.h>

class ExampleMaterial : public Pine::Material
{
public:
    ExampleMaterial(const Pine::Ref<Pine::Shader>& program)
        : Pine::Material(program)
    {}
    virtual ~ExampleMaterial() {};

    void SetColor(const glm::vec4& color)
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
    Pine::TMaterialUniform<glm::vec4> uColor = { "u_Color", glm::vec4(1) };
};