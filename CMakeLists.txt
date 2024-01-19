cmake_minimum_required(VERSION 3.25)
project(rrose_selavy)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_program(GLSLC glslc)
function(add_shader TARGET SHADER)
    set(current-shader-path ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER})

    get_filename_component(current-output-file ${SHADER}.spv NAME)
    set(current-output-path ${CMAKE_BINARY_DIR}/${current-output-file})
    get_filename_component(current-output-dir ${current-output-path} DIRECTORY)
    file(MAKE_DIRECTORY ${current-output-dir})

    add_custom_command(
            OUTPUT ${current-output-path}
            COMMAND ${GLSLC} -o ${current-output-path} ${current-shader-path}
            DEPENDS ${current-shader-path}
            IMPLICIT_DEPENDS CXX ${current-shader-path}
            VERBATIM
    )

    set_source_files_properties(${current-output-path} PROPERTIES GENERATED TRUE)
    target_sources(${TARGET} PRIVATE ${current-output-path})
endfunction(add_shader)

find_package(Vulkan REQUIRED)

add_executable(
        rrose_selavy
        source/main.cpp
        source/vk.cpp
        source/vk.h
        source/error.cpp
        source/error.h
        source/log.cpp
        source/log.h
)

target_compile_options(
        rrose_selavy
        PRIVATE -Wall -Werror -Wextra -Wpedantic
)

target_compile_definitions(
        rrose_selavy
        PRIVATE VULKAN_HPP_NO_CONSTRUCTORS # VULKAN_HPP_NO_EXCEPTIONS - needed for raii until 1.3.275 (2024-01-02)
)

target_include_directories(
        rrose_selavy
        PRIVATE ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(
        rrose_selavy
        Vulkan::Vulkan
)

add_shader(
        rrose_selavy
        source/main.comp
)