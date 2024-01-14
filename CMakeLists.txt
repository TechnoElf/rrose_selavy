cmake_minimum_required(VERSION 3.25)
project(rrose_selavy)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)

add_executable(
        rrose_selavy
        source/main.cpp
)

target_compile_options(
        rrose_selavy
        PRIVATE -Wall -Werror -Wextra -Wpedantic
)

target_compile_definitions(
        rrose_selavy
        PRIVATE VULKAN_HPP_NO_EXCEPTIONS
)

target_include_directories(
        rrose_selavy
        PRIVATE ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(
        rrose_selavy
        Vulkan::Vulkan
)