//
// Created by Ewan Arends on 17-5-2025.
//

#include "Swapchain.h"

#include <algorithm>


Swapchain::Swapchain(Window window, RenderManager::Handles *handles) {
    this->window = &window;
    this->handles = handles;
    uint32_t count;
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(handles->physical_device, window.get_surface(), &count, nullptr) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to get swapchain surface formats!");
    }
    std::vector<VkSurfaceFormatKHR> formats(count);
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(handles->physical_device, window.get_surface(), &count, formats.data())
        != VK_SUCCESS) {
        throw std::runtime_error("failed to get swapchain surface formats!");
    }

    for (uint32_t i = 0; i < count; i++) {
        for (auto format: swapchain_formats) {
            if (formats[i].format == format) {
                this->format = formats[i];
                break;
            }
            if (this->format.has_value())
                break;
        }
    }
    if (!format.has_value()) {
        throw std::runtime_error("failed to find surface supported swapchain format!");
    }
    extent = {};

    glfwGetFramebufferSize(window.get_window_handle(), reinterpret_cast<int *>(&extent.width),
                           reinterpret_cast<int *>(&extent.height));

    count = 0;
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(handles->physical_device, window.get_surface(), &count, nullptr) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to get swapchain presentation modes!");
    }
    std::vector<VkPresentModeKHR> present_modes(count);
    for (uint32_t i = 0; i < count; i++) {
        if (present_modes[i] == VK_PRESENT_MODE_FIFO_KHR) {
            present_mode = VK_PRESENT_MODE_FIFO_KHR;
        }
    }
    if (!present_mode.has_value()) {
        present_mode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    }

    VkSurfaceCapabilitiesKHR capabilites;
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(handles->physical_device, window.get_surface(),&capabilites) != VK_SUCCESS) {
        throw std::runtime_error("failed to get swapchain capabilities!");
    }

    image_count = std::clamp( preferred_image_count, capabilites.minImageCount, capabilites.maxImageCount);

}

void Swapchain::init(const Swapchain *old) {
    VkSwapchainCreateInfoKHR createInfo{VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    createInfo.surface = window->get_surface();
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = present_mode.value();
    createInfo.imageExtent = extent;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    createInfo.imageFormat = format.value().format;
    createInfo.imageArrayLayers = 1;
    createInfo.imageColorSpace = format.value().colorSpace;
    createInfo.minImageCount = image_count;
    createInfo.clipped = VK_FALSE;
    createInfo.queueFamilyIndexCount = 1;
    createInfo.pQueueFamilyIndices = &handles->indices.present;
    createInfo.oldSwapchain = old != nullptr ? old->swapchain_handle : VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(handles->device, &createInfo, nullptr, &swapchain_handle) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swapchain!");
    }

    uint32_t swapchain_image_count;
    if (vkGetSwapchainImagesKHR(handles->device, swapchain_handle, &swapchain_image_count, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("failed to get swapchain images!");
    }
    std::vector<VkImage> swapchain_images(swapchain_image_count);
    if (vkGetSwapchainImagesKHR(handles->device, swapchain_handle, &swapchain_image_count, swapchain_images.data()) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to get swapchain images!");
    }

    this->images[swapchain_image_count];
    for (int i = 0; i < (swapchain_image_count - 1); i++) {
        populate_image(images[i], &swapchain_images[i]);
    }
}

void Swapchain::populate_image(SwapchainImage &image, VkImage *value) const {
    image.image = *value;

    VkImageViewCreateInfo createInfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    createInfo.image = image.image;
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = format.value().format;
    createInfo.components = {
        VK_COMPONENT_SWIZZLE_R,
        VK_COMPONENT_SWIZZLE_G,
        VK_COMPONENT_SWIZZLE_B,
        VK_COMPONENT_SWIZZLE_A,
    };
    createInfo.subresourceRange = {
        VK_IMAGE_ASPECT_COLOR_BIT,
        0,
        1,
        0,
        1
    };
    if (vkCreateImageView(handles->device, &createInfo, nullptr, &image.image_view) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image view!");
    }

    VkFenceCreateInfo fence_info{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    if (vkCreateFence(handles->device, &fence_info, nullptr, &image.fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to create fence!");
    }

    VkSemaphoreCreateInfo semaphore_info{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    if (vkCreateSemaphore(handles->device, &semaphore_info, nullptr, &image.semaphore)) {
        throw std::runtime_error("failed to create semaphore!");
    }
}
