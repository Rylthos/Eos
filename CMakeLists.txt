cmake_minimum_required(VERSION 3.18)

project (Eos VERSION 0.1.0 DESCRIPTION "Vulkan Render Engine")

file (GLOB_RECURSE EOS_PROJECT_FILES CONFIGURE_DEPENDS "src/*.cpp" "src/*.hpp")
add_library(${PROJECT_NAME} SHARED ${EOS_PROJECT_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${PROJECT_NAME} PRIVATE .)

add_compile_definitions(EOS_PLATFORM_WINDOWS EOS_BUILD_DLL)
