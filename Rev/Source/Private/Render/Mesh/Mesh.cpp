#include "Rev/Render/Mesh/Mesh.h"

namespace Rev
{

Mesh::Mesh()
    : mName("")
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

}
