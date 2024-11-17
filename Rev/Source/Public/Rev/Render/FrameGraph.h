#pragma once
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHITexture.h"
#include "fg/FrameGraph.hpp"
#include "fg/Blackboard.hpp"


namespace Rev
{

struct FGraphTexture
{
	using Desc = FRHITextureDesc;

    FGraphTexture() = default;
    FGraphTexture(FGraphTexture&&) noexcept = default;

    void create(const Desc& InDesc, void*) {
        if(!TextureRHI)
            TextureRHI = GDynamicRHI->RHICreateTexture(InDesc);
    }
    void destroy(const Desc&, void*) {
        //TextureRHI.reset();
    }

    void preRead(const Desc&, uint32_t, void*) const {}
    void preWrite() const {}

    static const char* toString(const Desc&) { return "<I>texture</I>"; }

    Ref<FRHITexture> TextureRHI = nullptr;

};

struct FGraphPass
{
    using Desc = FRHIRenderPassDesc;

    FGraphPass() = default;
    FGraphPass(FGraphPass&&) noexcept = default;

    void create(const Desc& InDesc, void*) {
        RenderPassRHI = GDynamicRHI->RHICreateRenderPass(InDesc);
    }
    void destroy(const Desc&, void*) {
        RenderPassRHI.reset();
    }

    void preRead(const Desc&, uint32_t, void*) const {}
    void preWrite() const {}

    static const char* toString(const Desc&) { return "<I>render pass</I>"; }

    Ref<FRHIRenderPass> RenderPassRHI = nullptr;

};

}