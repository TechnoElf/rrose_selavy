/*
 * Created by janis on 2024-01-14
 */

#include <iostream>

#include <vulkan/vulkan.hpp>

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

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
    vk::ApplicationInfo app_info("rrose_selavy", 1, "rrose_selavy", 1, VK_API_VERSION_1_3);
    vk::StructureChain<vk::InstanceCreateInfo, vk::DebugUtilsMessengerCreateInfoEXT> instance_create_info(
        {
            {},
            &app_info},
        {
            {},
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
            &vk_debug_callback
        }
    );

    auto [res, instance] = vk::createInstance(instance_create_info.get());

    instance.destroy();

    return 0;
}