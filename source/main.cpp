/*
 * Created by janis on 2024-01-14
 */

#include <string>

#include "vk.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
    const uint32_t w = 4;
    const uint32_t h = 4;

    rr::Vulkan vk = rr::Vulkan::try_init().unwrap();
    rr::Log::info("Vulkan initialised");
    std::vector<rr::RGBA> result = vk.run(w, h).unwrap();
    rr::Log::info("Done");
    for (uint32_t y = 0; y < h; y++) {
        for (uint32_t x = 0; x < w; x++) {
            rr::RGBA col = result[y * w + x];
            rr::Log::info(("(" + std::to_string(x) + ", " + std::to_string(y) + ") - (" + std::to_string(col.r) + ", " + std::to_string(col.g) + ", " + std::to_string(col.b) + ", "
                + std::to_string(col.a) + ")").c_str());
        }
    }

    return 0;
}