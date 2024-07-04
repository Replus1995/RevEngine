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
class REV_API Material
{
public:
	Material(const Ref<Shader>& program);
	virtual ~Material();
	
	/*const Ref<Shader>& GetShader() const
	{
		return mProgram;
	}*/

	void SetDomain(EMaterialDomain domain);
	EMaterialDomain GetDomain() const;

	void Bind();
	void Unbind();

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