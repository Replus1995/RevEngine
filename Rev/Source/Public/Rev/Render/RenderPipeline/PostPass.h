#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RenderPipeline/RenderPass.h"

namespace Rev
{
class FRHIVertexArray;
class FPostPass : public FRHIRenderPass
{
public:
	FPostPass(std::string_view InName);
	virtual ~FPostPass();
	virtual void BeginPass() override;
	virtual void EndPass() override;
	void SetRenderTarget(const Ref<FRHIRenderTarget>& InTarget);
	static void DrawQuad();

private:
	friend class FRenderPipeline;
	static void CreateVertexData();
	static void DestroyVertexData();
	static const Ref<FRHIVertexArray>& GetVertexData();

private:
	bool mInPass = false;
};

}