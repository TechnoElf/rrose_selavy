/*
 * Created by janis on 2024-01-17
 */

#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include "error.h"

namespace rr {
    struct RGBA {
        float r;
        float g;
        float b;
        float a;
    };

    struct ParametersUBO {
        uint32_t width;
        uint32_t height;
    };

    struct Vulkan {
        vk::raii::Instance instance;
        vk::raii::DebugUtilsMessengerEXT debug_utils_messenger;
        std::vector<vk::raii::PhysicalDevice> physical_devices;
        vk::raii::Device device;
        vk::raii::Queue queue;
        vk::raii::ShaderModule shader_module;
        vk::raii::PipelineLayout pipeline_layout;
        vk::raii::PipelineCache pipeline_cache;
        vk::raii::Pipeline pipeline;
        vk::raii::Buffer buffer;
        vk::raii::DeviceMemory buffer_device_memory;
        vk::raii::CommandPool command_pool;
        vk::raii::CommandBuffer command_buffer;
        vk::raii::DescriptorPool descriptor_pool;
        vk::raii::DescriptorSetLayout descriptor_set_layout;
        std::vector<vk::raii::DescriptorSet> descriptor_sets;

    private:

    public:
        static Result<Vulkan> try_init();

        Result<std::vector<RGBA>> run(uint32_t w, uint32_t h);
    };
}