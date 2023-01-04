cmake_minimum_required(VERSION 3.18)

project(Eos VERSION 0.1.0 DESCRIPTION "Vulkan Render Engine")

set (EOS_PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

option(EOS_BUILD_EXAMPLES "Build the EOS Example programs" ON)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EOS_PROJECT_DIR}/bin)
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set (EOS_BUILDING_STANDALONE TRUE)
endif ()

add_subdirectory(vendor)

add_subdirectory(eos)

if (EOS_BUILDING_STANDALONE)
    message ("Building EOS Examples")
    add_subdirectory(examples)
endif ()

