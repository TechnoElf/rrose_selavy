/*
 * Created by janis on 2024-01-14
 */

#include <string>
#include <fstream>
#include <iostream>

#include "vk.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
    const uint32_t w = 71;
    const uint32_t h = 64;

    rr::Vulkan vk = rr::Vulkan::try_init().unwrap();
    rr::Log::info("Vulkan initialised");
    std::vector<rr::RGBA> result = vk.run(w, h).unwrap();
    rr::Log::info("Done");

    std::ofstream out("img.ppm", std::ios::binary);

    out << "P6" << std::endl;
    out << std::to_string(w) << " " << std::to_string(h) << std::endl;
    out << "255" << std::endl;

    for (uint32_t y = 0; y < h; y++) {
        for (uint32_t x = 0; x < w; x++) {
            rr::RGBA col = result[y * w + x];
            uint8_t pixel[3] = {
                static_cast<uint8_t>(col.r * 255.0),
                static_cast<uint8_t>(col.g * 255.0),
                static_cast<uint8_t>(col.b * 255.0)
            };
            out.write(reinterpret_cast<const char*>(pixel), 3);
        }
    }

    out.close();

    rr::Log::info("Saved");

    return 0;
}