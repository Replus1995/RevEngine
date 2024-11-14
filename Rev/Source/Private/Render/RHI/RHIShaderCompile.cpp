#include "Rev/Render/RHI/RHIShaderCompile.h"
#include "Rev/Core/Hash.h"

namespace Rev
{

void FRHIShaderCompileOptions::AddMacro(const std::string& InMacro)
{
	Macros += ";" + InMacro;
}

uint32 FRHIShaderCompileOptions::GetHash() const
{
	return FCityHash::Gen(Macros);
}

}


