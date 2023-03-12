# GLM
set (GLM_QUIET OFF CACHE BOOL "" FORCE)
set (GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
add_subdirectory(glm)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)
set_target_properties(glfw PROPERTIES FOLDER vendor)

# Vk-Bootstrap
add_subdirectory(vk-bootstrap)

# VulkanMemoryAllocator
add_subdirectory(VulkanMemoryAllocator)
set_target_properties(VulkanMemoryAllocator PROPERTIES CXX_STANDARD 20)

# spdlog
add_subdirectory(spdlog)

# STB
add_subdirectory(stb)

# Imgui
project (Imgui)

find_package(Vulkan REQUIRED)

file (GLOB_RECURSE imguiFiles CONFIGURE_DEPENDS "Imgui/*.cpp" "Imgui/*.h")
add_library(Imgui STATIC ${imguiFiles})
target_include_directories(Imgui PUBLIC "Imgui/" "Imgui/backends")

target_link_libraries(Imgui PUBLIC
    glfw
    Vulkan::Vulkan
    )
