find_package (Vulkan REQUIRED)

set (CMAKE_CXX_STANDARD 20)

file (GLOB_RECURSE EOS_PROJECT_FILES CONFIGURE_DEPENDS
    "${EOS_PROJECT_DIR}/eos/*.cpp" "${EOS_PROJECT_DIR}/eos/*.hpp
     ${EOS_PRKECT_DIR}/eos/*.inl")

add_library(Eos SHARED ${EOS_PROJECT_FILES})

set_target_properties(Eos PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(Eos PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(Eos BEFORE PRIVATE ${EOS_PROJECT_DIR}/eos)

set (EOS_LINKER "")
if (WIN32)
    set (EOS_LINKER "")
elseif (UNIX)
    set (EOS_LINKER "-fPIC")
endif ()

target_link_libraries(Eos
    PRIVATE
        "-Wall -Wextra -Wc++17-extensions -g"

    PUBLIC
        Vulkan::Vulkan
        glm::glm
        glfw
        vk-bootstrap::vk-bootstrap
        VulkanMemoryAllocator
        spdlog
        stb
        Imgui
    )

target_precompile_headers(Eos PRIVATE ${EOS_PROJECT_DIR}/eos/Eos/EosPCH.hpp)
target_compile_definitions(Eos PRIVATE EOS_BUILD_DLL)
target_compile_definitions(Eos PUBLIC GLFW_INCLUDE_VULKAN)

if (WIN32)
    target_compile_definitions(Eos PRIVATE EOS_PLATFORM_WINDOWS)
elseif (UNIX)
    target_compile_definitions(Eos PRIVATE EOS_PLATFORM_LINUX)
endif ()
