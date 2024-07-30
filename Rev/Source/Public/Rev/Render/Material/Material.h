#pragma once
#include "Rev/Core/Base.h"

namespace Rev
{

enum EMaterialDomain : uint8
{
	MD_Surface,
	MD_PostProcess
};

class FRHIShaderProgram;
class REV_API Material
{
public:
	Material();
	virtual ~Material();
	
	/**
	* @brief Create shader program based on ShadingModel
	*/
	virtual void Compile() = 0;
	virtual void PreDraw(); //Bind Program
	virtual void PostDraw(); //Unbind Program
	//const Ref<FRHIShaderProgram>& GetProgram() const { return mProgram; }

	EMaterialDomain GetDomain() const { return mDomain; }
protected:
	Ref<FRHIShaderProgram> mProgram = nullptr;
	EMaterialDomain mDomain = MD_Surface;
};

}