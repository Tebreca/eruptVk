//
// Created by Ewan Arends on 17-5-2025.
//

#include "RenderManager.h"

#include "Pipeline.h"
#include "Swapchain.h"
#include "../Window.h"

RenderManager::RenderManager(const vkb::Device &device, const vkb::PhysicalDevice &physical_device,
                             const vkb::Instance &instance, Window window) {
    this->device = device;
    this->physical_device = physical_device;
    this->instance = instance;
    this->window = window;


    auto graphics_queue_result = device.get_queue(vkb::QueueType::graphics);
    auto graphics_queue_index_result = device.get_queue_index(vkb::QueueType::graphics);

    auto transfer_queue_result = device.get_queue(vkb::QueueType::transfer);
    auto transfer_queue_index_result = device.get_queue_index(vkb::QueueType::transfer);

    auto present_queue_result = device.get_queue(vkb::QueueType::present);
    auto present_queue_index_result = device.get_queue_index(vkb::QueueType::present);

    if (!graphics_queue_result || !transfer_queue_result || !graphics_queue_index_result || !
        transfer_queue_index_result) {
        throw std::runtime_error("failed to create queues");
    }

    handles.graphics_queue = graphics_queue_result.value();
    handles.indices.graphics = graphics_queue_index_result.value();
    handles.transfer_queue = transfer_queue_result.value();
    handles.indices.transfer = transfer_queue_index_result.value();
    handles.present_queue = present_queue_result.value();
    handles.indices.graphics = graphics_queue_index_result.value();

    handles.device = device.device;
    handles.physical_device = physical_device.physical_device;
    handles.instance = instance.instance;

    Swapchain swapchain(window, &handles);
    swapchain.init(nullptr);

    Pipeline pipeline(&handles);
    pipeline.init();


}
