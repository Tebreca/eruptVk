//
// Created by Ewan Arends on 17-5-2025.
//

#ifndef GRAPHICSCONSTANTS_H
#define GRAPHICSCONSTANTS_H
#include <cstdint>
#include <vulkan/vulkan_core.h>

namespace constants {
    inline constexpr uint32_t max_frames_in_flight = 3;
    inline constexpr VkFormat swapchain_formats[] = {
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_FORMAT_R16G16B16_SFLOAT
    };
    inline constexpr uint32_t preferred_image_count = 3;


    inline constexpr auto vertex_shader = "shaders/vertex.spv";
    inline constexpr auto fragment_shader = "shaders/frag.spv";
};


#endif //GRAPHICSCONSTANTS_H
