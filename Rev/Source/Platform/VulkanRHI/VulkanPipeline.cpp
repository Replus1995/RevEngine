#include "VulkanPipeline.h"
#include "VulkanDynamicRHI.h"
#include "VulkanRenderPass.h"
#include "VulkanShader.h"
#include "VulkanState.h"
#include "Core/VulkanEnum.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

class FVulkanPipelineBuilder
{
public:
    static VkPipeline BuildGraphics(VkDevice InDevice, VkPipelineLayout InLayout,
        const FRHIGraphicsPipelineStateDesc& InStateDesc,
        const FVulkanRenderPass* RenderPass,
        const FVulkanShaderProgram* InProgram);

private:
    static VkPipelineInputAssemblyStateCreateInfo MakeInputAssemblyStateInfo(VkPrimitiveTopology InTopology);
    static VkPipelineTessellationStateCreateInfo MakeTessellationStateInfo();
    static VkPipelineViewportStateCreateInfo MakeViewportStateInfo();
    static VkPipelineMultisampleStateCreateInfo MakeMultisampleStateInfo(VkSampleCountFlagBits SampleCountFlag, bool bAlphaToCoverage);
    static VkPipelineColorBlendStateCreateInfo MakeColorBlendStateInfo(const VkPipelineColorBlendAttachmentState* InColorBlendStates, uint32 InNumColorBlendStates);
    static VkPipelineDynamicStateCreateInfo MakeDynamicStateInfo(const VkDynamicState* InDynaimcStates, uint32 InNumDynaimcStates);
    static VkPipelineRenderingCreateInfo MakeRenderingInfo(const VkFormat* InColorFomats, uint32 InNumColorFormats, VkFormat InDepthFormat);

};

VkPipeline FVulkanPipelineBuilder::BuildGraphics(VkDevice InDevice, VkPipelineLayout InLayout, 
    const FRHIGraphicsPipelineStateDesc& InStateDesc,
    const FVulkanRenderPass* RenderPass, 
    const FVulkanShaderProgram* InProgram)
{
    const FRHIRenderPassDesc& RenderPassDesc = RenderPass->GetDesc();
    uint32 ColorAttachmentCount = RenderPassDesc.NumColorRenderTargets;
    REV_CORE_ASSERT(ColorAttachmentCount <= RTA_MaxColorAttachments);

    REV_CORE_ASSERT(InStateDesc.RasterizerState);
    REV_CORE_ASSERT(InStateDesc.DepthStencilState);
    REV_CORE_ASSERT(InStateDesc.ColorBlendState);
    REV_CORE_ASSERT(InStateDesc.VertexInputState);

    FVulkanRasterizerState* RasterizerStateRHI = static_cast<FVulkanRasterizerState*>(InStateDesc.RasterizerState);
    FVulkanDepthStencilState* DepthStencilStateRHI = static_cast<FVulkanDepthStencilState*>(InStateDesc.DepthStencilState);
    FVulkanColorBlendState* ColorBlendStateRHI = static_cast<FVulkanColorBlendState*>(InStateDesc.ColorBlendState);
    FVulkanVertexInputState* VertexInputStataRHI = static_cast<FVulkanVertexInputState*>(InStateDesc.VertexInputState);

    VkPipelineShaderStageCreateInfo ShaderStageInfos[SS_NumGraphics];
    uint32 NumShaderStageInfos = InProgram->GenShaderStageInfo(ShaderStageInfos);

    VkDynamicState DynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    //std::vector<VkDynamicState> DynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY };

    VkFormat ColorFormats[RTA_MaxColorAttachments];
    for (uint32 i = 0; i < ColorAttachmentCount; i++)
    {
        REV_CORE_ASSERT(RenderPassDesc.ColorRenderTargets[i].ColorTarget);
        ColorFormats[i] = (VkFormat)GPixelFormats[RenderPassDesc.ColorRenderTargets[i].ColorTarget->GetFormat()].PlatformFormat;
    }
    VkFormat DepthFormat = VK_FORMAT_UNDEFINED;
    if (RenderPassDesc.DepthStencilRenderTarget.DepthStencilTarget)
    {
        DepthFormat = (VkFormat)GPixelFormats[RenderPassDesc.DepthStencilRenderTarget.DepthStencilTarget->GetFormat()].PlatformFormat;
    }

    bool bEnableAlphaToCoverage = InStateDesc.NumSamples > 1 && ColorBlendStateRHI->Desc.bUseAlphaToCoverage;

    VkPipelineInputAssemblyStateCreateInfo InputAssemblyState = MakeInputAssemblyStateInfo(FVulkanEnum::Translate(InStateDesc.PrimitiveTopology));
    VkPipelineTessellationStateCreateInfo TessellationState = MakeTessellationStateInfo();
    VkPipelineViewportStateCreateInfo ViewportState = MakeViewportStateInfo();
    VkPipelineMultisampleStateCreateInfo MultisampleState = MakeMultisampleStateInfo((VkSampleCountFlagBits)InStateDesc.NumSamples, bEnableAlphaToCoverage);
    VkPipelineColorBlendStateCreateInfo ColorBlendState = MakeColorBlendStateInfo(ColorBlendStateRHI->Attachments, ColorAttachmentCount);
    VkPipelineDynamicStateCreateInfo DynamicState = MakeDynamicStateInfo(DynamicStates, sizeof(DynamicStates) / sizeof(VkDynamicState));
    VkPipelineRenderingCreateInfo Rendering = MakeRenderingInfo(ColorFormats, ColorAttachmentCount, DepthFormat);

    // build the actual pipeline
    // we now use all of the info structs we have been writing into into this one
    // to create the pipeline
    VkGraphicsPipelineCreateInfo PipelineInfo{};
    PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    // connect the renderInfo to the pNext extension mechanism
    PipelineInfo.pNext = &Rendering;

    PipelineInfo.stageCount = NumShaderStageInfos;
    PipelineInfo.pStages = ShaderStageInfos;
    PipelineInfo.pVertexInputState = &VertexInputStataRHI->VertexInputState;
    PipelineInfo.pInputAssemblyState = &InputAssemblyState;
    PipelineInfo.pViewportState = &ViewportState;
    PipelineInfo.pRasterizationState = &RasterizerStateRHI->RasterizerState;
    PipelineInfo.pMultisampleState = &MultisampleState;
    PipelineInfo.pDepthStencilState = &DepthStencilStateRHI->DepthStencilState;
    PipelineInfo.pColorBlendState = &ColorBlendState;
    PipelineInfo.pDynamicState = &DynamicState;
    PipelineInfo.layout = InLayout;
    PipelineInfo.renderPass = (VkRenderPass)RenderPass->GetNativeHandle();
    PipelineInfo.subpass = 0;


    // its easy to error out on create graphics pipeline, so we handle it a bit
   // better than the common VK_CHECK case
    VkPipeline NewPipeline;
    REV_VK_CHECK_THROW(vkCreateGraphicsPipelines(InDevice, VK_NULL_HANDLE, 1, &PipelineInfo, nullptr, &NewPipeline), "failed to create pipeline");
    return NewPipeline;
}

VkPipelineInputAssemblyStateCreateInfo FVulkanPipelineBuilder::MakeInputAssemblyStateInfo(VkPrimitiveTopology InTopology)
{
    VkPipelineInputAssemblyStateCreateInfo StateInfo{};
    StateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    StateInfo.topology = InTopology;
    StateInfo.primitiveRestartEnable = VK_FALSE;
    return StateInfo;
}

VkPipelineTessellationStateCreateInfo FVulkanPipelineBuilder::MakeTessellationStateInfo()
{
   VkPipelineTessellationStateCreateInfo StateInfo{};
   StateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
   StateInfo.patchControlPoints = 0;
   return StateInfo;
}

VkPipelineViewportStateCreateInfo FVulkanPipelineBuilder::MakeViewportStateInfo()
{
    // make viewport state from our stored viewport and scissor.
    // at the moment we wont support multiple viewports or scissors
    VkPipelineViewportStateCreateInfo StateInfo{};
    StateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    StateInfo.viewportCount = 1;
    StateInfo.scissorCount = 1;
    return StateInfo;
}

VkPipelineMultisampleStateCreateInfo FVulkanPipelineBuilder::MakeMultisampleStateInfo(VkSampleCountFlagBits SampleCountFlag, bool bAlphaToCoverage)
{
    VkPipelineMultisampleStateCreateInfo StateInfo{};
    StateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    StateInfo.sampleShadingEnable = VK_FALSE;
    StateInfo.rasterizationSamples = SampleCountFlag;
    StateInfo.minSampleShading = 1.0f;
    StateInfo.pSampleMask = nullptr;
    StateInfo.alphaToCoverageEnable = VK_FALSE;
    StateInfo.alphaToOneEnable = bAlphaToCoverage;
    return StateInfo;
}

VkPipelineColorBlendStateCreateInfo FVulkanPipelineBuilder::MakeColorBlendStateInfo(const VkPipelineColorBlendAttachmentState* InColorBlendStates, uint32 InNumColorBlendStates)
{
    VkPipelineColorBlendStateCreateInfo StateInfo{};
    StateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    StateInfo.logicOpEnable = VK_FALSE;
    StateInfo.logicOp = VK_LOGIC_OP_COPY;
    StateInfo.pAttachments = InColorBlendStates;
    StateInfo.attachmentCount = InNumColorBlendStates;

    StateInfo.blendConstants[0] = 1.0f;
    StateInfo.blendConstants[1] = 1.0f;
    StateInfo.blendConstants[2] = 1.0f;
    StateInfo.blendConstants[3] = 1.0f;

    /*for (size_t i = 0; i < 4; i++)
    {
        StateInfo.blendConstants[i] = InStateRHI.BlendConstants[i];
    }*/

    return StateInfo;
}

VkPipelineDynamicStateCreateInfo FVulkanPipelineBuilder::MakeDynamicStateInfo(const VkDynamicState* InDynaimcStates, uint32 InNumDynaimcStates)
{
    VkPipelineDynamicStateCreateInfo StateInfo{};
    StateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    StateInfo.pDynamicStates = InDynaimcStates;
    StateInfo.dynamicStateCount = InNumDynaimcStates;
    return StateInfo;
}

VkPipelineRenderingCreateInfo FVulkanPipelineBuilder::MakeRenderingInfo(const VkFormat* InColorFomats, uint32 InNumColorFormats, VkFormat InDepthFormat)
{
   VkPipelineRenderingCreateInfo StateInfo{};
   StateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
   StateInfo.pColorAttachmentFormats = InColorFomats;
   StateInfo.colorAttachmentCount = InNumColorFormats;
   StateInfo.depthAttachmentFormat = InDepthFormat;
   StateInfo.stencilAttachmentFormat = InDepthFormat;
   return StateInfo;
}

FVulkanPipelineLayout::FVulkanPipelineLayout()
{
}

FVulkanPipelineLayout::~FVulkanPipelineLayout()
{
    Release();
}

void FVulkanPipelineLayout::Build(const VkDescriptorSetLayoutBinding* InBindingInfo, uint32 InBindingsCount)
{
    VkDescriptorSetLayoutCreateInfo DescSetLayoutCreateInfo{};
    DescSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    DescSetLayoutCreateInfo.pNext = NULL;
    DescSetLayoutCreateInfo.flags = 0;
    DescSetLayoutCreateInfo.bindingCount = InBindingsCount;
    DescSetLayoutCreateInfo.pBindings = InBindingInfo;

    REV_VK_CHECK_THROW(vkCreateDescriptorSetLayout(FVulkanDynamicRHI::GetDevice(), &DescSetLayoutCreateInfo, nullptr, &DescriptorSetLayout), "failed to create descriptor set layout");

    VkPipelineLayoutCreateInfo GraphicsLayoutCreateInfo{};
    GraphicsLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    GraphicsLayoutCreateInfo.setLayoutCount = 1;
    GraphicsLayoutCreateInfo.pSetLayouts = &DescriptorSetLayout;
    GraphicsLayoutCreateInfo.pushConstantRangeCount = 0; // Optional
    GraphicsLayoutCreateInfo.pPushConstantRanges = nullptr; // Optional

    REV_VK_CHECK_THROW(vkCreatePipelineLayout(FVulkanDynamicRHI::GetDevice(), &GraphicsLayoutCreateInfo, nullptr, &PipelineLayout), "failed to create pipeline layout");
}

void FVulkanPipelineLayout::Release()
{
    if (PipelineLayout)
        vkDestroyPipelineLayout(FVulkanDynamicRHI::GetDevice(), PipelineLayout, nullptr);
    if (DescriptorSetLayout)
        vkDestroyDescriptorSetLayout(FVulkanDynamicRHI::GetDevice(), DescriptorSetLayout, nullptr);
}

FVulkanPipeline::FVulkanPipeline()
{
}

FVulkanPipeline::~FVulkanPipeline()
{
    Release();
}

void FVulkanPipeline::BuildGraphics(FVulkanPipelineLayout* InLayout, const FRHIGraphicsPipelineStateDesc& InStateDesc, const FVulkanRenderPass* RenderPass, const FVulkanShaderProgram* InProgram)
{
    REV_CORE_ASSERT(InLayout != nullptr);
    REV_CORE_ASSERT(InLayout->PipelineLayout != VK_NULL_HANDLE);

    Release();
    Pipeline = FVulkanPipelineBuilder::BuildGraphics(FVulkanDynamicRHI::GetDevice(), InLayout->PipelineLayout, InStateDesc, RenderPass, InProgram);
    PipelineLayout = InLayout;
}


void FVulkanPipeline::Release()
{
    if (Pipeline)
        vkDestroyPipeline(FVulkanDynamicRHI::GetDevice(), Pipeline, nullptr);
}



FVulkanGraphicsPipelineCache::~FVulkanGraphicsPipelineCache()
{
    ClearAll();
}

FVulkanPipeline* FVulkanGraphicsPipelineCache::GetOrCreatePipeline(
    const FRHIGraphicsPipelineStateDesc& InStateDesc,
    const FVulkanRenderPass* InRenderPass, 
    const FVulkanShaderProgram* InProgram)
{
    REV_CORE_ASSERT(InStateDesc.VertexInputState);

    FVulkanGraphicsPipelineDesc PipelineDesc(InStateDesc, InRenderPass, InProgram);
    FVulkanPipeline* pPipeline = nullptr;
    if (auto PipelineIter = mPipelineCache.find(PipelineDesc); PipelineIter != mPipelineCache.end())
    {
        pPipeline = PipelineIter->second.get();
    }
    if (!pPipeline)
    {
        FVulkanPipelineLayout* pLayout = nullptr;
        {
            FVulkanPipelineLayoutDesc LayoutDesc(InProgram);
            if (auto LayoutIter = mLayoutCache.find(LayoutDesc); LayoutIter != mLayoutCache.end())
            {
                pLayout = LayoutIter->second.get();
            }
            if (!pLayout)
            {
                Scope<FVulkanPipelineLayout> NewLayout = CreateScope<FVulkanPipelineLayout>();
                NewLayout->Build(LayoutDesc.Bindings, LayoutDesc.NumBindings);
                auto LayoutIter = mLayoutCache.emplace(LayoutDesc, std::move(NewLayout)).first;
                pLayout = LayoutIter->second.get();
            }
        }

        Scope<FVulkanPipeline> NewPipeline = CreateScope<FVulkanPipeline>();
        NewPipeline->BuildGraphics(pLayout, InStateDesc, InRenderPass, InProgram);
        auto PipelineIter = mPipelineCache.emplace(PipelineDesc, std::move(NewPipeline)).first;
        pPipeline = PipelineIter->second.get();
    }

    return pPipeline;
}

void FVulkanGraphicsPipelineCache::ClearAll()
{
    mPipelineCache.clear();
    mLayoutCache.clear();
}

bool operator==(const FVulkanPipelineLayoutDesc& A, const FVulkanPipelineLayoutDesc& B)
{
    bool bSame = A.NumBindings == B.NumBindings;
    for (uint32 i = 0; i < A.NumBindings; i++)
    {
        bSame &= A.Bindings[i] == B.Bindings[i];
        if(!bSame)
            break;
    }
    return bSame;
}

bool operator==(const FVulkanGraphicsPipelineDesc& A, const FVulkanGraphicsPipelineDesc& B)
{
    bool bSame = A.PipelineState == B.PipelineState && A.RenderPass == B.RenderPass;
    for (int i = 0; i < 5; i++)
    {
        bSame &= A.ShaderModules[i] == B.ShaderModules[i];
    }
    return bSame;
}

FVulkanPipelineLayoutDesc::FVulkanPipelineLayoutDesc(const FVulkanShaderProgram* InProgram)
{
    REV_CORE_ASSERT(InProgram != nullptr);

    NumBindings = InProgram->GenLayoutBindings(Bindings);
}

FVulkanGraphicsPipelineDesc::FVulkanGraphicsPipelineDesc(const FRHIGraphicsPipelineStateDesc& InPipelineState, const FVulkanRenderPass* InRenderPass, const FVulkanShaderProgram* InProgram)
    : PipelineState(InPipelineState)
{
    REV_CORE_ASSERT(InRenderPass != nullptr);
    REV_CORE_ASSERT(InProgram != nullptr);

    RenderPass = (VkRenderPass)InRenderPass->GetNativeHandle();
    for (uint8 i = SS_Vertex; i < SS_NumGraphics; i++)
    {
        auto& Shader = InProgram->GetShaders()[i];
        if (Shader)
        {
            ShaderModules[i] = (VkShaderModule)Shader->GetNativeHandle();
        }
    }

}

}

bool operator==(const VkDescriptorSetLayoutBinding& A, const VkDescriptorSetLayoutBinding& B)
{
    bool bSame = A.binding == B.binding &&
        A.descriptorType == B.descriptorType &&
        A.descriptorCount == B.descriptorCount &&
        A.stageFlags == B.stageFlags &&
        A.pImmutableSamplers == B.pImmutableSamplers;
    return false;
}
