#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <bits/atomic_base.h>

#include "Window.h"
#include "../dependencies/vkb/src/VkBootstrap.h"
#include "render/RenderManager.h"

using namespace constants;

int main() {
    Window window;
    window.init();

    vkb::InstanceBuilder builder;
    auto inst_result = builder.set_app_name(game_name)
            .set_app_version(version)
            .set_engine_name("Quant")
            .set_engine_version(version)
            .request_validation_layers()
            .use_default_debug_messenger()
            .build();
    if (!inst_result) {
        /* report */
    }
    vkb::Instance vkb_inst = inst_result.value();

    vkb::PhysicalDeviceSelector selector{vkb_inst};

    window.setupSurface(&vkb_inst.instance);

    auto phys_result = selector.set_surface(window.get_surface())
            .set_minimum_version(1, 4)
            .require_dedicated_transfer_queue()
            .select();
    if (!phys_result) {
        /* report */
    }

    const auto& physical_device= phys_result.value();
    vkb::DeviceBuilder device_builder{physical_device};
    auto dev_result = device_builder
            .build();
    if (!dev_result) {
        /* report */
    }
    vkb::Device vkb_device = dev_result.value();


    RenderManager render_manager(vkb_device, physical_device, vkb_inst, window);


}
