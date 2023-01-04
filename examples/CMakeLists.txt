cmake_minimum_required (VERSION 3.18)

# Build the Shaders
add_custom_target(EOS_EXAMPLES ALL COMMENT "Building Shaders")
find_program(glslc_exec NAMES glslc HINTS Vulkan::glslc)

add_dependencies(EOS_EXAMPLES BasicTriangle)

set (InputShadersFile "${PROJECT_SOURCE_DIR}/examples/Shaders")
set (OutputShadersFile "${PROJECT_SOURCE_DIR}/bin/Shaders")

file (GLOB_RECURSE Shaders CONFIGURE_DEPENDS 
    ${InputShadersFile}/*.vert
    ${InputShadersFile}/*.frag)

foreach (Shader ${Shaders})
    get_filename_component(File ${Shader} NAME)

    add_custom_command(
        TARGET EOS_EXAMPLES PRE_BUILD
        COMMAND
            ${glslc_exec}
            ${InputShadersFile}/${File}
            -o ${InputShadersFile}/${File}.spv
        COMMENT "Compiled ${File} To ${File}.spv"
        )

    add_custom_command(
        TARGET EOS_EXAMPLES POST_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy
            ${InputShadersFile}/${File}.spv
            ${OutputShadersFile}/${File}.spv
        COMMENT "Copied ${InputShadersFile}/${File}.spv to ${OutputShadersFile}/${File}.spv"
        )
endforeach()

# Build the examples
set (CMAKE_CXX_STANDARD 20)

project (BasicTriangle)

add_executable(BasicTriangle BasicTriangle.cpp)

set (ExamplesLinker "-std=c++2a -g -Wall -Wextra -Wpedantic")

target_include_directories(BasicTriangle PRIVATE "${EOS_PROJECT_DIR}/eos")
target_link_libraries(BasicTriangle PRIVATE ${ExamplesLinker} PUBLIC Eos)

target_compile_definitions(BasicTriangle PUBLIC EOS_PLATFORM_WINDOWS)
