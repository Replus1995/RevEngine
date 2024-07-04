#include "Rev/Render/Mesh/Mesh.h"

namespace Rev
{

Mesh::Mesh()
    : mName("")
    , mMode(MeshDrawMode::TRIANGLES)
{
}

void Mesh::SetName(const std::string& InName)
{
    mName = InName;
}

const std::string& Mesh::GetName() const
{
    return mName;
}

void Mesh::SetDrawMode(MeshDrawMode InMode)
{
    mMode = InMode;
}

}
