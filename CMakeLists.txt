cmake_minimum_required(VERSION 3.18)

project(Eos VERSION 0.1.0 DESCRIPTION "Vulkan Render Engine")

set (EOS_PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

set (CMAKE_VERSION 20)

option(EOS_BUILD_EXAMPLES "Build the EOS Example programs" OFF)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EOS_PROJECT_DIR}/bin)
    set (EOS_BUILDING_STANDALONE TRUE)
endif ()

find_package(Vulkan REQUIRED)

if (WIN32)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
elseif (UNIX)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=c++20")
endif ()

add_subdirectory(vendor)

add_subdirectory(eos)

if (${EOS_BUILDING_STANDALONE} OR ${EOS_BUILD_EXAMPLES})
    message ("Building EOS Examples")
    add_subdirectory(examples)
endif ()

