cmake_minimum_required(VERSION 3.18)

project (Eos VERSION 0.1.0 DESCRIPTION "Vulkan Render Engine")

find_package(Vulkan REQUIRED)

file (GLOB_RECURSE EOS_PROJECT_FILES CONFIGURE_DEPENDS "src/*.cpp" "src/*.hpp")
add_library(${PROJECT_NAME} SHARED ${EOS_PROJECT_FILES})

add_subdirectory(vendor)

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${PROJECT_NAME} PRIVATE .)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Vulkan::Vulkan
        glfw
    )

target_compile_definitions(${PROJECT_NAME} PRIVATE EOS_PLATFORM_WINDOWS EOS_BUILD_DLL)
