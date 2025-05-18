//
// Created by Ewan Arends on 17-5-2025.
//
#pragma once

#include <vulkan/vulkan.h> //required for GLFW
#define  GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "constants/GameConstants.h"

using namespace constants;

#ifndef WINDOW_H
#define WINDOW_H


class Window {
    GLFWwindow *window_handle;

    VkSurfaceKHR surface;

public:
    [[nodiscard]] GLFWwindow * get_window_handle() const {
        return window_handle;
    }

    [[nodiscard]] VkSurfaceKHR get_surface() const {
        return surface;
    }

    void init();


    void setupSurface(VkInstance *instance);

};


#endif //WINDOW_H
