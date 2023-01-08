cmake_minimum_required (VERSION 3.18)

# Build the Shaders
add_custom_target(EOS_EXAMPLES ALL COMMENT "Building Shaders")
find_program(glslc_exec NAMES glslc HINTS Vulkan::glslc)

add_dependencies(EOS_EXAMPLES BasicTriangle)

MACRO(SUBDIRLIST result curdir)
    FILE (GLOB children RELATIVE ${curdir} ${curdir}/*)
    set(dirlist "")
    foreach (child ${children})
        IF(IS_DIRECTORY ${curdir}/${child})
            LIST(APPEND dirlist ${child})
        ENDIF()
    endforeach()
    SET(${result} ${dirlist})
endmacro()

SUBDIRLIST(AllExamples "${PROJECT_SOURCE_DIR}/examples/res")
foreach (Example ${AllExamples})
    set (InputShadersFile "${PROJECT_SOURCE_DIR}/examples/res/${Example}/Shaders")
    set (OutputShadersFile "${PROJECT_SOURCE_DIR}/bin/res/${Example}/Shaders")

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
endforeach()

# Build the examples
set (CMAKE_CXX_STANDARD 20)
set (ExamplesLinker "-std=c++2a -g -Wall -Wextra -Wpedantic")

file (GLOB_RECURSE AllExamples CONFIGURE_DEPENDS *.cpp)
foreach (Example ${AllExamples})
    get_filename_component(Filename ${Example} NAME_WLE)

    project (${Filename})

    add_executable(${Filename} ${Filename}.cpp)

    target_include_directories(${Filename} PRIVATE "${EOS_PROJECT_DIR}/eos")
    target_link_libraries(${Filename} PRIVATE ${ExamplesLinker} PUBLIC Eos)

    target_compile_definitions(${Filename} PUBLIC EOS_PLATFORM_WINDOWS)
endforeach ()
