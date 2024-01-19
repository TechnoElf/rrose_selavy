/*
 * Created by janis on 2024-01-17
 */

#include "vk.h"

#include <iostream>
#include <fstream>

namespace rr {
    vk::Bool32 vk_debug_callback([[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                 [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT type,
                                 VkDebugUtilsMessengerCallbackDataEXT const* data,
                                 [[maybe_unused]] void* user_data) {
        auto sev = vk::DebugUtilsMessageSeverityFlagBitsEXT(severity);
        std::cerr << "[VK] " << to_string(sev) << ": " << data->pMessage << std::endl;
        if (sev == vk::DebugUtilsMessageSeverityFlagBitsEXT::eError) {
            abort();
        }
        return vk::False;
    }

    template<typename T>
    T vk_check(vk::ResultValue<T> res) {
        if (res.result == vk::Result::eSuccess) {
            return res.value;
        } else {
            std::cerr << "[VK] Error: Returned " << to_string(res.result) << std::endl;
            abort();
        }
    }

    Result<Vulkan> Vulkan::try_init() {
        try {
            vk::raii::Context context;

            vk::ApplicationInfo app_info = {
                .pApplicationName = "rrose_selavy",
                .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                .pEngineName = "rrose_selavy",
                .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                .apiVersion = VK_API_VERSION_1_3
            };

            std::vector<const char*> instance_extensions = {VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME};
            std::vector<const char*> instance_layers = {"VK_LAYER_KHRONOS_validation"};

            vk::InstanceCreateInfo instance_create_info = {
                .flags = vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR,
                .pApplicationInfo = &app_info,
                .enabledLayerCount = static_cast<uint32_t>(instance_layers.size()),
                .ppEnabledLayerNames = instance_layers.data(),
                .enabledExtensionCount = static_cast<uint32_t>(instance_extensions.size()),
                .ppEnabledExtensionNames = instance_extensions.data()
            };

            vk::DebugUtilsMessengerCreateInfoEXT debug_utils_messenger_create_info = {
                .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
                .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
                .pfnUserCallback = &vk_debug_callback
            };

            vk::raii::Instance instance = context.createInstance(vk::StructureChain<vk::InstanceCreateInfo, vk::DebugUtilsMessengerCreateInfoEXT>(
                instance_create_info,
                debug_utils_messenger_create_info
            ).get());

            vk::raii::DebugUtilsMessengerEXT debug_utils_messenger = instance.createDebugUtilsMessengerEXT({
                                                                                                               .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
                                                                                                                   | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
                                                                                                                   | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
                                                                                                               .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
                                                                                                                   | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
                                                                                                                   | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
                                                                                                               .pfnUserCallback = &vk_debug_callback
                                                                                                           });

            std::vector<vk::raii::PhysicalDevice> physical_devices = instance.enumeratePhysicalDevices();

            std::vector<float> queue_priorities = {1.0};

            std::vector<vk::DeviceQueueCreateInfo> queue_create_infos = {vk::DeviceQueueCreateInfo{
                .queueFamilyIndex = 0,
                .queueCount = static_cast<uint32_t>(queue_priorities.size()),
                .pQueuePriorities = queue_priorities.data()
            }};

            vk::raii::Device device = physical_devices[0].createDevice({
                                                                           .queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size()),
                                                                           .pQueueCreateInfos = queue_create_infos.data()
                                                                       });

            vk::raii::Queue queue = device.getQueue(0, 0);

            std::vector<uint32_t> queue_family_indices = {0};

            vk::raii::Buffer buffer = device.createBuffer({
                                                              .size = 4 * 4,
                                                              .usage = vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eTransferSrc,
                                                              .sharingMode = vk::SharingMode::eExclusive,
                                                              .queueFamilyIndexCount = static_cast<uint32_t>(queue_family_indices.size()),
                                                              .pQueueFamilyIndices = queue_family_indices.data(),
                                                          });

            vk::MemoryRequirements memory_requirements = buffer.getMemoryRequirements();
            uint32_t memory_type = 0;
            vk::PhysicalDeviceMemoryProperties memory_properties = physical_devices[0].getMemoryProperties();
            for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
                if (memory_requirements.memoryTypeBits & (1 << i)) {
                    if (memory_properties.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eHostVisible &&
                        memory_properties.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eHostCoherent) {
                        memory_type = i;
                    }
                }
            }

            vk::raii::DeviceMemory device_memory = device.allocateMemory({
                                                                             .allocationSize = 4 * 4,
                                                                             .memoryTypeIndex = memory_type,
                                                                         });

            buffer.bindMemory(*device_memory, 0);

            vk::raii::CommandPool command_pool = device.createCommandPool({
                                                                              .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                                                                              .queueFamilyIndex = 0
                                                                          });

            std::vector<vk::raii::CommandBuffer> command_buffers = device.allocateCommandBuffers({
                                                                                                     .commandPool = *command_pool,
                                                                                                     .level = vk::CommandBufferLevel::ePrimary,
                                                                                                     .commandBufferCount = 1
                                                                                                 });

            std::vector<vk::DescriptorPoolSize> descriptor_pool_sizes = {
                {
                    .type = vk::DescriptorType::eStorageBuffer,
                    .descriptorCount = 1
                }
            };

            vk::raii::DescriptorPool descriptor_pool = device.createDescriptorPool({
                                                                                       .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
                                                                                       .maxSets = 1,
                                                                                       .poolSizeCount = static_cast<uint32_t>(descriptor_pool_sizes.size()),
                                                                                       .pPoolSizes = descriptor_pool_sizes.data()
                                                                                   });

            std::vector<vk::DescriptorSetLayoutBinding> descriptor_set_layout_bindings = {
                {
                    .binding = 0,
                    .descriptorType = vk::DescriptorType::eStorageBuffer,
                    .descriptorCount = 1,
                    .stageFlags = vk::ShaderStageFlagBits::eCompute
                }
            };

            vk::raii::DescriptorSetLayout descriptor_set_layout = device.createDescriptorSetLayout({
                                                                                                       .bindingCount = static_cast<uint32_t>(descriptor_set_layout_bindings.size()),
                                                                                                       .pBindings = descriptor_set_layout_bindings.data()
                                                                                                   });

            std::vector<vk::raii::DescriptorSet> descriptor_sets = device.allocateDescriptorSets({
                                                                                                     .descriptorPool = *descriptor_pool,
                                                                                                     .descriptorSetCount = 1,
                                                                                                     .pSetLayouts = &*descriptor_set_layout
                                                                                                 });

            vk::DescriptorBufferInfo descriptor_buffer_info = {
                .buffer = *buffer,
                .offset = 0,
                .range = 4 * 4
            };

            std::vector<vk::WriteDescriptorSet> write_descriptor_sets = {
                {
                    .dstSet = *descriptor_sets[0],
                    .dstBinding = 0,
                    .dstArrayElement = 0,
                    .descriptorCount = 1,
                    .descriptorType = vk::DescriptorType::eStorageBuffer,
                    .pBufferInfo = &descriptor_buffer_info
                }
            };

            device.updateDescriptorSets(write_descriptor_sets, {});

            std::ifstream file = std::ifstream("main.comp.spv", std::ios::ate | std::ios::binary);
            size_t size = file.tellg();
            std::vector<char> buf = std::vector<char>(size);
            file.seekg(0);
            file.read(buf.data(), size);
            file.close();

            vk::raii::ShaderModule shader_module = device.createShaderModule({
                                                                                 .codeSize = buf.size(),
                                                                                 .pCode = reinterpret_cast<const uint32_t*>(buf.data())
                                                                             });

            vk::raii::PipelineLayout pipeline_layout = device.createPipelineLayout({
                                                                                       .setLayoutCount = 1,
                                                                                       .pSetLayouts = &*descriptor_set_layout
                                                                                   });

            vk::raii::PipelineCache pipeline_cache = device.createPipelineCache({});

            vk::raii::Pipeline pipeline = device.createComputePipeline(pipeline_cache, {
                .stage = {
                    .stage = vk::ShaderStageFlagBits::eCompute,
                    .module = *shader_module,
                    .pName = "main"
                },
                .layout = *pipeline_layout
            });

            return Result<Vulkan>({
                                      .instance = std::move(instance),
                                      .debug_utils_messenger = std::move(debug_utils_messenger),
                                      .physical_devices = std::move(physical_devices),
                                      .device = std::move(device),
                                      .queue = std::move(queue),
                                      .shader_module = std::move(shader_module),
                                      .pipeline_layout = std::move(pipeline_layout),
                                      .pipeline_cache = std::move(pipeline_cache),
                                      .pipeline = std::move(pipeline),
                                      .buffer = std::move(buffer),
                                      .device_memory = std::move(device_memory),
                                      .command_pool = std::move(command_pool),
                                      .command_buffer = std::move(command_buffers[0]),
                                      .descriptor_pool = std::move(descriptor_pool),
                                      .descriptor_set_layout = std::move(descriptor_set_layout),
                                      .descriptor_sets = std::move(descriptor_sets)
                                  });
        } catch (std::exception& e) {
            return Result<Vulkan>({
                                      .kind = ErrorKind::Vulkan
                                  });
        }
    }

    Result<std::vector<uint32_t>> Vulkan::run() {
        try {
            std::vector<uint32_t> data = {9, 9, 9, 9};

            void* buf = this->device_memory.mapMemory(0, 4 * 4);
            memcpy(buf, data.data(), 4 * 4);
            this->device_memory.unmapMemory();

            this->command_buffer.reset();
            this->command_buffer.begin({});
            this->command_buffer.bindPipeline(vk::PipelineBindPoint::eCompute, *this->pipeline);
            this->command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, *this->pipeline_layout, 0, {*this->descriptor_sets[0]}, nullptr);
            this->command_buffer.dispatch(4, 1, 1);
            this->command_buffer.end();

            std::vector<vk::CommandBuffer> command_buffers = {
                *this->command_buffer
            };

            std::vector<vk::SubmitInfo> queue_create_infos = {vk::SubmitInfo{
                .commandBufferCount = static_cast<uint32_t>(command_buffers.size()),
                .pCommandBuffers = command_buffers.data()
            }};

            this->queue.submit(queue_create_infos);

            this->device.waitIdle();

            buf = this->device_memory.mapMemory(0, 4 * 4);
            memcpy(data.data(), buf, 4 * 4);
            this->device_memory.unmapMemory();

            return Result<std::vector<uint32_t>>(std::move(data));
        } catch (std::exception& e) {
            return Result<std::vector<uint32_t>>({
                                       .kind = ErrorKind::Vulkan
                                   });
        }
    }
}
