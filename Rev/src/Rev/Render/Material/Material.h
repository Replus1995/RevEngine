#pragma once
#include "Rev/Render/Material/MaterialUniform.h"
#include "Rev/Math/Maths.h"
#include <unordered_map>

namespace Rev
{

enum class EMaterialDomain
{
	Opaque,
	Transparent,
	Unlit
};

class MaterialUniform;
class Material
{
public:
	REV_API Material(const Ref<Shader>& program);
	REV_API virtual ~Material();
	
	/*const Ref<Shader>& GetShader() const
	{
		return mProgram;
	}*/

	REV_API void SetDomain(EMaterialDomain domain);
	REV_API EMaterialDomain GetDomain() const;

	REV_API void Bind();
	REV_API void Unbind();

protected:
	virtual void UploadUniform();
	friend class StaticMeshRenderProxy;
protected:
	Ref<Shader> mProgram = nullptr;
	EMaterialDomain mDomain = EMaterialDomain::Opaque;

	//Uniform
	TMaterialUniform<Math::FMatrix4> uTransform = { "Transform", Math::FMatrix4(1) };
};

}