#pragma once
#include "Pine/Render/MaterialUniform.h"
#include <unordered_map>

namespace Pine
{
enum class EMaterialDomain
{
	Opaque,
	Transparent,
	Unlit
};

class MaterialUniform;
class PINE_API Material
{
public:
	Material(const Ref<Shader>& program);
	virtual ~Material();
	
	/*const Ref<Shader>& GetShader() const
	{
		return mProgram;
	}*/

	void SetDomain(EMaterialDomain domain)
	{
		mDomain = domain;
	}

	EMaterialDomain GetDomain() const
	{
		return mDomain;
	}

	void Bind();
	void Unbind();

protected:
	virtual void UploadUniform();
	friend class StaticMeshRenderProxy;
protected:
	Ref<Shader> mProgram = nullptr;
	EMaterialDomain mDomain = EMaterialDomain::Opaque;

	//Uniform
	TMaterialUniform<glm::mat4> uTransform = { "u_Transform", glm::mat4(1) };
};

}