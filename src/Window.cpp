//
// Created by Ewan Arends on 17-5-2025.
//

#include "Window.h"

void Window::init() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    // hidden, constant size window with no api for vulkan insertion
    window_handle = glfwCreateWindow(800, 600, game_name, nullptr, nullptr);
}

void Window::setupSurface(VkInstance*instance) {
    glfwCreateWindowSurface(*instance, window_handle, nullptr, &surface);
    glfwShowWindow(window_handle);

}
