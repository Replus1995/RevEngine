#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RenderPipeline/RenderPass.h"

namespace Rev
{
class FRHIVertexArray;
class FFullScreenPass : public FRenderPass
{
public:
	FFullScreenPass(std::string_view InName);
	virtual ~FFullScreenPass();

protected:
	void DrawQuad();
private:
	friend class FRenderPipeline;
	static void CreatePrimitive();
	static void DestroyPrimitive();
};

}