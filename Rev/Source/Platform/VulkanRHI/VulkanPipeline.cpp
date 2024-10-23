#include "VulkanPipeline.h"
#include "VulkanDynamicRHI.h"
#include "VulkanRenderPass.h"
#include "VulkanShader.h"
#include "VulkanPrimitive.h"
#include "VulkanState.h"
#include "Core/VulkanDefines.h"
#include "Core/VulkanEnum.h"
#include "Rev/Core/Assert.h"
#include "Rev/Core/Hash.h"

namespace Rev
{

class FVulkanPipelineBuilder
{
public:
    static VkPipeline BuildGraphics(VkDevice InDevice, VkPipelineLayout InLayout,
        const FRHIGraphicsPipelineStateDesc& InStateDesc,
        const FVulkanRenderPass* RenderPass,
        const FVulkanShaderProgram* InProgram,
        const FVulkanPrimitive* InPrimitive);

private:
    static VkPipelineInputAssemblyStateCreateInfo MakeInputAssemblyStateInfo(VkPrimitiveTopology InTopology);
    static VkPipelineTessellationStateCreateInfo MakeTessellationStateInfo();
    static VkPipelineViewportStateCreateInfo MakeViewportStateInfo();
    static VkPipelineMultisampleStateCreateInfo MakeMultisampleStateInfo();
    static VkPipelineColorBlendStateCreateInfo MakeColorBlendStateInfo(const VkPipelineColorBlendAttachmentState* InColorBlendStates, uint32 InNumColorBlendStates);
    static VkPipelineDynamicStateCreateInfo MakeDynamicStateInfo(const VkDynamicState* InDynaimcStates, uint32 InNumDynaimcStates);
    static VkPipelineRenderingCreateInfo MakeRenderingInfo(const VkFormat* InColorFomats, uint32 InNumColorFormats, VkFormat InDepthFormat);

};

VkPipeline FVulkanPipelineBuilder::BuildGraphics(VkDevice InDevice, VkPipelineLayout InLayout, 
    const FRHIGraphicsPipelineStateDesc& InStateDesc,
    const FVulkanRenderPass* RenderPass, 
    const FVulkanShaderProgram* InProgram, 
    const FVulkanPrimitive* InPrimitive)
{
    const FRHIRenderPassDesc& RenderPassDesc = RenderPass->GetDesc();
    uint32 ColorAttachmentCount = RenderPassDesc.NumColorAttachments;
    REV_CORE_ASSERT(ColorAttachmentCount <= RTA_MaxColorAttachments);

    FVulkanRasterizerState RasterizerStateRHI(InStateDesc.RasterizerStateDesc);
    FVulkanDepthStencilState DepthStencilStateRHI(InStateDesc.DepthStencilStateDesc);
    FVulkanColorBlendState ColorBlendStateRHI(InStateDesc.ColorBlendStateDesc);
    FVulkanVertexInputState VertexInputStataRHI(InStateDesc.VertexInputStateDesc);
    auto ShaderStageInfo = InProgram->GenShaderStageInfo();

    VkDynamicState DynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    //std::vector<VkDynamicState> DynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY };

    VkFormat ColorFormats[RTA_MaxColorAttachments];
    for (uint32 i = 0; i < ColorAttachmentCount; i++)
    {
        ColorFormats[i] = (VkFormat)GPixelFormats[RenderPassDesc.ColorAttachments[i].Format].PlatformFormat;
    }
    VkFormat DepthFormat = (VkFormat)GPixelFormats[RenderPassDesc.DepthStencilAttchment.Format].PlatformFormat;
    
    VkPipelineVertexInputStateCreateInfo PrimitiveVertexInputState{};
    if (InPrimitive)
    {
        PrimitiveVertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        PrimitiveVertexInputState.vertexBindingDescriptionCount = (uint32_t)InPrimitive->GetBindingDescs().size();
        PrimitiveVertexInputState.pVertexBindingDescriptions = InPrimitive->GetBindingDescs().data();
        PrimitiveVertexInputState.vertexAttributeDescriptionCount = (uint32_t)InPrimitive->GetAttributeDescs().size();
        PrimitiveVertexInputState.pVertexAttributeDescriptions = InPrimitive->GetAttributeDescs().data();
    }


    VkPipelineInputAssemblyStateCreateInfo InputAssemblyState = MakeInputAssemblyStateInfo(FVulkanEnum::Translate(InStateDesc.PrimitiveTopology));
    VkPipelineTessellationStateCreateInfo TessellationState = MakeTessellationStateInfo();
    VkPipelineViewportStateCreateInfo ViewportState = MakeViewportStateInfo();
    VkPipelineMultisampleStateCreateInfo MultisampleState = MakeMultisampleStateInfo();
    VkPipelineColorBlendStateCreateInfo ColorBlendState = MakeColorBlendStateInfo(ColorBlendStateRHI.Attachments, ColorAttachmentCount);
    VkPipelineDynamicStateCreateInfo DynamicState = MakeDynamicStateInfo(DynamicStates, sizeof(DynamicStates) / sizeof(VkDynamicState));
    VkPipelineRenderingCreateInfo Rendering = MakeRenderingInfo(ColorFormats, ColorAttachmentCount, DepthFormat);

    // build the actual pipeline
    // we now use all of the info structs we have been writing into into this one
    // to create the pipeline
    VkGraphicsPipelineCreateInfo PipelineInfo{};
    PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    // connect the renderInfo to the pNext extension mechanism
    PipelineInfo.pNext = &Rendering;

    PipelineInfo.stageCount = (uint32_t)ShaderStageInfo.size();
    PipelineInfo.pStages = ShaderStageInfo.data();
    PipelineInfo.pVertexInputState = InPrimitive ? &PrimitiveVertexInputState : &VertexInputStataRHI.VertexInputState;
    PipelineInfo.pInputAssemblyState = &InputAssemblyState;
    PipelineInfo.pViewportState = &ViewportState;
    PipelineInfo.pRasterizationState = &RasterizerStateRHI.RasterizerState;
    PipelineInfo.pMultisampleState = &MultisampleState;
    PipelineInfo.pDepthStencilState = &DepthStencilStateRHI.DepthStencilState;
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

VkPipelineMultisampleStateCreateInfo FVulkanPipelineBuilder::MakeMultisampleStateInfo()
{
    VkPipelineMultisampleStateCreateInfo StateInfo{};
    StateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    StateInfo.sampleShadingEnable = VK_FALSE;
    StateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    StateInfo.minSampleShading = 1.0f;
    StateInfo.pSampleMask = nullptr;
    StateInfo.alphaToCoverageEnable = VK_FALSE;
    StateInfo.alphaToOneEnable = VK_FALSE;
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

void FVulkanPipelineLayout::Build(const std::vector<VkDescriptorSetLayoutBinding>& InBindingInfo)
{
    VkDescriptorSetLayoutCreateInfo DescSetLayoutCreateInfo{};
    DescSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    DescSetLayoutCreateInfo.pNext = NULL;
    DescSetLayoutCreateInfo.flags = 0;
    DescSetLayoutCreateInfo.bindingCount = (uint32_t)InBindingInfo.size();
    DescSetLayoutCreateInfo.pBindings = InBindingInfo.data();

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

void FVulkanPipeline::BuildGraphics(FVulkanPipelineLayout* InLayout, const FRHIGraphicsPipelineStateDesc& InStateDesc, const FVulkanRenderPass* RenderPass, const FVulkanShaderProgram* InProgram, const FVulkanPrimitive* InPrimitive)
{
    REV_CORE_ASSERT(InLayout != nullptr);
    REV_CORE_ASSERT(InLayout->PipelineLayout != VK_NULL_HANDLE);

    Release();
    Pipeline = FVulkanPipelineBuilder::BuildGraphics(FVulkanDynamicRHI::GetDevice(), InLayout->PipelineLayout, InStateDesc, RenderPass, InProgram, InPrimitive);
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

FVulkanPipeline* FVulkanGraphicsPipelineCache::GetOrCreatePipeline(const FRHIGraphicsPipelineStateDesc& InStateDesc,
    const FVulkanRenderPass* InRenderPass, 
    const FVulkanShaderProgram* InProgram, 
    const FVulkanPrimitive* InPrimitive)
{
    
    FCacheKey PipelineCacheKey(InStateDesc, InRenderPass, InProgram, InPrimitive);
    FVulkanPipeline* pPipeline = nullptr;
    if (auto PipelineIter = mPipelineCache.find(PipelineCacheKey); PipelineIter != mPipelineCache.end())
    {
        pPipeline = PipelineIter->second.get();
    }
    if (!pPipeline)
    {
        FVulkanPipelineLayout* pLayout = nullptr;
        {
            std::vector<VkDescriptorSetLayoutBinding> Bindings = InProgram->GenLayoutBindings();
            uint32 LayoutCacheKey = FCityHash::Gen(Bindings.data(), Bindings.size() * sizeof(VkDescriptorSetLayoutBinding));

            if (auto LayoutIter = mLayoutCache.find(LayoutCacheKey); LayoutIter != mLayoutCache.end())
            {
                pLayout = LayoutIter->second.get();
            }
            if (!pLayout)
            {
                Scope<FVulkanPipelineLayout> NewLayout = CreateScope<FVulkanPipelineLayout>();
                NewLayout->Build(Bindings);
                auto LayoutIter = mLayoutCache.emplace(LayoutCacheKey, std::move(NewLayout)).first;
                pLayout = LayoutIter->second.get();
            }
        }

        Scope<FVulkanPipeline> NewPipeline = CreateScope<FVulkanPipeline>();
        NewPipeline->BuildGraphics(pLayout, InStateDesc, InRenderPass, InProgram, InPrimitive);
        auto PipelineIter = mPipelineCache.emplace(PipelineCacheKey, std::move(NewPipeline)).first;
        pPipeline = PipelineIter->second.get();
    }

    return pPipeline;
}

void FVulkanGraphicsPipelineCache::ClearAll()
{
    mPipelineCache.clear();
    mLayoutCache.clear();
}

FVulkanGraphicsPipelineCache::FCacheKey::FCacheKey(const FRHIGraphicsPipelineStateDesc& InStateDesc, 
    const FVulkanRenderPass* InRenderPass, 
    const FVulkanShaderProgram* InProgram, 
    const FVulkanPrimitive* InPrimitive)
{
    StateHash = FCityHash::Gen(&InStateDesc, sizeof(InStateDesc));
    RenderPass = (VkRenderPass)InRenderPass->GetNativeHandle();

    for (uint8 i = (uint8)ERHIShaderStage::Vertex; i < (uint8)ERHIShaderStage::Compute; i++)
    {
        const auto& pShaderRHI = InProgram->GetShaders()[(ERHIShaderStage)i];
        FVulkanShader* pShaderVk = static_cast<FVulkanShader*>(pShaderRHI.get());
        if (!pShaderVk)
            continue;
        ShaderHash[i - 1] = pShaderVk->GetHash();
    }
    if(InPrimitive)
        VertexHash = InPrimitive->GetDescHash(); //todo: get vertex data hash from shader

    SecondaryHash = FCityHash::Gen(this, sizeof(FCacheKey) - sizeof(uint32));
}

bool operator==(const FVulkanGraphicsPipelineCache::FCacheKey& L, const FVulkanGraphicsPipelineCache::FCacheKey& R)
{
    if (L.SecondaryHash == R.SecondaryHash)
    {
        bool bSame = L.StateHash == R.StateHash &&
            L.RenderPass == R.RenderPass &&
            L.VertexHash == R.VertexHash;

        if (bSame)
        {
            for (uint8 i = 0; i < 5; i++)
            {
                bSame &= L.ShaderHash[i] == R.ShaderHash[i];
                if (!bSame)
                    return false;
            }
        }

        return bSame;
    }

    return false;
}

bool operator<(const FVulkanGraphicsPipelineCache::FCacheKey& L, const FVulkanGraphicsPipelineCache::FCacheKey& R)
{
    return L.SecondaryHash < R.SecondaryHash;
}

}