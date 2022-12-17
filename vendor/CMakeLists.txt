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

# spdlog
add_subdirectory(spdlog)
