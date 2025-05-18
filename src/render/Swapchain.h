//
// Created by Ewan Arends on 17-5-2025.
//

#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H
#include <optional>
#include <vulkan/vulkan_core.h>

#include "RenderManager.h"
#include "../constants/GameConstants.h"
#include "../Window.h"
#include "../constants/GraphicsConstants.h"

using namespace constants;

class Swapchain {
    VkSwapchainKHR swapchain_handle;
    Window *window;
    RenderManager::Handles *handles;

    struct SwapchainImage {
        VkImage image;
        VkImageView image_view;

        VkFence fence;
        VkSemaphore semaphore;
    };

    SwapchainImage images[0];
    std::optional<VkSurfaceFormatKHR> format;
    VkExtent2D extent;
    std::optional<VkPresentModeKHR> present_mode;
    uint32_t image_count;

    void populate_image(SwapchainImage &image, VkImage *value) const;

public:
    explicit Swapchain(Window window, RenderManager::Handles *handles);


    void init(const Swapchain *old);
};


#endif //SWAPCHAIN_H
