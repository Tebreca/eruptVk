//
// Created by Ewan Arends on 18-5-2025.
//

#ifndef PIPELINE_H
#define PIPELINE_H
#include <vulkan/vulkan.h>

#include "RenderManager.h"

using namespace constants;

class Pipeline {

    RenderManager::Handles *handles;

public:
    explicit Pipeline(RenderManager::Handles *handles)
        : handles(handles) {
    }

private:


    void setup_pipeline(VkRenderPass renderpass);

    void setup_layout();

    void load_shaders();

    VkShaderModule vert_shader;
    VkShaderModule frag_shader;

public:
    VkPipelineLayout layout;
    VkPipeline pipeline;


    void init();
};


#endif //PIPELINE_H
