/*
 * Created by janis on 2024-01-14
 */

#include <string>

#include "vk.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
    rr::Vulkan vk = rr::Vulkan::try_init().unwrap();
    rr::Log::info("Vulkan initialised");
    std::vector<uint32_t> result = vk.run().unwrap();
    rr::Log::info(("Done " + std::to_string(result[0]) + " " + std::to_string(result[1]) + " " + std::to_string(result[2]) + " " + std::to_string(result[3])).c_str());

    return 0;
}