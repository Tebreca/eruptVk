//
// Created by Ewan Arends on 17-5-2025.
//

#ifndef RENDERMANGER_H
#define RENDERMANGER_H
#include "vk_mem_alloc.h"
#include "src/VkBootstrap.h"

#include "../Window.h"

class RenderManager {
    RenderManager() = default;

    vkb::Device device;
    vkb::PhysicalDevice physical_device;
    vkb::Instance instance;
    VkQueue_T *graphics_queue;
    uint32_t graphics_queue_index;
    VkQueue_T *transfer_queue;
    uint32_t transfer_queue_index;
    Window window;

public:
    RenderManager(const vkb::Device &device, const vkb::PhysicalDevice &physical_device, const vkb::Instance &instance,
                  Window window);

    struct Handles {
        struct Indices {
            uint32_t graphics;
            uint32_t transfer;
            uint32_t present;
        };

        VkInstance instance;
        VkPhysicalDevice physical_device;
        VkDevice device;
        VkQueue graphics_queue;
        VkQueue transfer_queue;
        VkQueue present_queue;
        Indices indices;
    } handles;
};


#endif //RENDERMANGER_H
