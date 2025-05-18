//
// Created by Ewan Arends on 18-5-2025.
//

#include "Pipeline.h"

#include <filesystem>
#include <vector>

#include "../constants/GraphicsConstants.h"

void Pipeline::setup_pipeline(VkRenderPass renderpass) {
    VkGraphicsPipelineCreateInfo pipelineInfo = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};

    //Shader stages
    VkPipelineShaderStageCreateInfo shaderStages[] = {
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = vert_shader,
            .pName = "main",
            nullptr
        },
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = frag_shader,
            .pName = "main",
            nullptr
        }
    };
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;

    //Vertex input
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    pipelineInfo.pVertexInputState = &vertexInputInfo;

    //Input assembly
    VkPipelineInputAssemblyStateCreateInfo assembly_state_create_info = {
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO
    };
    assembly_state_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    assembly_state_create_info.primitiveRestartEnable = VK_FALSE;
    pipelineInfo.pInputAssemblyState = &assembly_state_create_info;

    //Dynamic
    VkDynamicState dynamic_states[] = {
        VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamic_state = {VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
    dynamic_state.dynamicStateCount = 2;
    dynamic_state.pDynamicStates = dynamic_states;
    pipelineInfo.pDynamicState = &dynamic_state;

    //Tesselation
    VkPipelineTessellationStateCreateInfo tesselation_state = {
        VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO
    };
    pipelineInfo.pTessellationState = &tesselation_state;

    //Rasterisation
    VkPipelineRasterizationStateCreateInfo rasterization_state = {
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO
    };
    /* For now no fancy rendering adventures */
    rasterization_state.depthBiasEnable = VK_FALSE;
    rasterization_state.cullMode = VK_CULL_MODE_NONE;
    rasterization_state.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterization_state.lineWidth = 1.0f;
    rasterization_state.polygonMode = VK_POLYGON_MODE_FILL;
    rasterization_state.rasterizerDiscardEnable = VK_FALSE;
    pipelineInfo.pRasterizationState = &rasterization_state;

    // Multisampling
    VkPipelineMultisampleStateCreateInfo multisample_state = {VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    pipelineInfo.pMultisampleState = &multisample_state;

    // Depth stencil
    VkPipelineDepthStencilStateCreateInfo depth_stencil_state = {
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO
    };
    depth_stencil_state.depthBoundsTestEnable = VK_FALSE;
    depth_stencil_state.depthTestEnable = VK_FALSE;
    pipelineInfo.pDepthStencilState = &depth_stencil_state;

    // Colour blend
    VkPipelineColorBlendStateCreateInfo color_blend_state = {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
    color_blend_state.logicOpEnable = VK_TRUE;
    color_blend_state.logicOp = VK_LOGIC_OP_COPY;
    color_blend_state.blendConstants[0] = 1.0f;
    color_blend_state.blendConstants[1] = 1.0f;
    color_blend_state.blendConstants[2] = 1.0f;
    color_blend_state.blendConstants[3] = 0.0f;
    color_blend_state.attachmentCount = 1;
    VkPipelineColorBlendAttachmentState attachment;
    attachment.alphaBlendOp = VK_BLEND_OP_ADD;
    attachment.colorBlendOp = VK_BLEND_OP_ADD;
    attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    color_blend_state.pAttachments = &attachment;
    pipelineInfo.pColorBlendState = &color_blend_state;

    pipelineInfo.layout = layout;
    pipelineInfo.renderPass = renderpass;
    pipelineInfo.subpass = 0;

}


void Pipeline::setup_layout() {
    VkPipelineLayoutCreateInfo create_info = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    /* For now there's no descriptors*/
    if (vkCreatePipelineLayout(handles->device, &create_info, nullptr, &layout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

void shader(VkDevice device, const char *fname, VkShaderModule *destination) {
    FILE *iobuf = std::fopen(fname, "r");
    if (iobuf == nullptr) {
        throw std::runtime_error("Could not open shader file");
    }
    std::fseek(iobuf, 0, SEEK_END);
    int size = std::ftell(iobuf) + 1;
    rewind(iobuf);
    std::vector<char> file(size);
    fread(file.data(), sizeof(char), size, iobuf);
    fclose(iobuf);

    VkShaderModuleCreateInfo info = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
    info.codeSize = size;
    info.pCode = reinterpret_cast<const uint32_t *>(file.data());

    if (vkCreateShaderModule(device, &info, nullptr, destination) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module");
    }
}

void Pipeline::load_shaders() {
    shader(handles->device, fragment_shader, &frag_shader);
    shader(handles->device, vertex_shader, &vert_shader);
}


void Pipeline::init() {
    setup_layout();
    load_shaders();
    setup_pipeline(TODO);
}
