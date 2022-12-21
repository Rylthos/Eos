cmake_minimum_required(VERSION 3.18)

project (Eos VERSION 0.1.0 DESCRIPTION "Vulkan Render Engine")

find_package(Vulkan REQUIRED)

file (GLOB_RECURSE EOS_PROJECT_FILES CONFIGURE_DEPENDS "src/*.cpp" "src/*.hpp")
add_library(${PROJECT_NAME} SHARED ${EOS_PROJECT_FILES})

target_include_directories(${PROJECT_NAME} BEFORE PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_subdirectory(vendor)

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -Wno-unknown-pragmas \
        -Wno-unused-variable -Wno-nullability-extension \
        -Wno-gnu-zero-variadic-macro-arguments -Wno-nullability-completeness \
        -Wno-missing-field-initializers"
    )

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Vulkan::Vulkan
        glm::glm
        glfw
        vk-bootstrap::vk-bootstrap
        VulkanMemoryAllocator
        spdlog
    )

target_precompile_headers(${PROJECT_NAME} PRIVATE src/Eos/EosPCH.hpp)
target_compile_definitions(${PROJECT_NAME} PRIVATE EOS_PLATFORM_WINDOWS EOS_BUILD_DLL)
target_compile_definitions(${PROJECT_NAME} PUBLIC GLFW_INCLUDE_VULKAN)
