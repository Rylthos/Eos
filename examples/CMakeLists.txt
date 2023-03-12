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

set (CMAKE_CXX_STANDARD 20)
set (ExamplesLinker "-std=c++2a -g -Wall -Wextra -Wpedantic")

SUBDIRLIST(AllExampleNames "${EOS_PROJECT_DIR}/examples/")
foreach (Example ${AllExampleNames})
    # Compile Example
    get_filename_component(ExampleName ${Example} NAME_WLE)
    file (GLOB Files CONFIGURE_DEPENDS "${EOS_PROJECT_DIR}/examples/${ExampleName}/*.cpp")

    project (${ExampleName})

    add_executable(${ExampleName} ${Files})

    target_include_directories(${ExampleName} PRIVATE "${EOS_PROJECT_DIR}/eos")
    target_link_libraries(${ExampleName} PRIVATE ${ExamplesLinker} PUBLIC Eos)

    if (WIN32)
        target_compile_definitions(${ExampleName} PUBLIC EOS_PLATFORM_WINDOWS)
    elseif (UNIX)
        target_compile_definitions(${ExampleName} PUBLIC EOS_PLATFORM_LINUX)
    endif ()

    set (InputResFile "${EOS_PROJECT_DIR}/examples/${ExampleName}/res")
    set (OutputResFile "${EOS_PROJECT_DIR}/bin/res/${ExampleName}")
    set (InputShadersFile "${InputResFile}/Shaders")
    set (OutputShadersFile "${OutputResFile}/Shaders")

    # Compile Shaders and move Textures
    if (EXISTS ${InputResFile}/Textures)
        file(COPY ${InputResFile}/Textures/ DESTINATION ${OutputResFile}/Textures/)
    endif ()

    file (GLOB_RECURSE Shaders CONFIGURE_DEPENDS 
        ${InputShadersFile}/*.vert
        ${InputShadersFile}/*.frag
        ${InputShadersFile}/*.comp)


    if (NOT EXISTS ${OutputShadersFile})
        add_custom_command(
            TARGET EOS_EXAMPLES POST_BUILD
            COMMAND
            ${CMAKE_COMMAND} -E make_directory
            ${OutputShadersFile}
            )
    endif()

    foreach (Shader ${Shaders})
        get_filename_component (File ${Shader} NAME)

        # Check if the file has been modified since the program was last Built
        # If modified then rebuild, else skip
        set (CacheValue ${InputShadersFile}/${File})

        file (TIMESTAMP ${Shader} Time)
        set (PreviousTime $CACHE{${CacheValue}})

        if (NOT DEFINED PreviousTime)
            set (PreviousTime "0")
        endif()

        if (${Time} STREQUAL ${PreviousTime} AND EXISTS "${OutputShadersFile}/${File}.spv")
            continue ()
        else()
            set (${CacheValue} ${Time} CACHE STRING "Time Data" FORCE)
        endif()

        add_custom_command(
            TARGET EOS_EXAMPLES POST_BUILD
            COMMAND
            ${glslc_exec}
            ${InputShadersFile}/${File}
            -o ${OutputShadersFile}/${File}.spv
            COMMENT "Compiled ${InputShadersFile}/${File} To ${OutputShadersFile}/${File}.spv"
            )
        #
        # add_custom_command(
        #     TARGET EOS_EXAMPLES POST_BUILD
        #     COMMAND
        #     ${CMAKE_COMMAND} -E copy
        #     ${InputShadersFile}/${File}.spv
        #     ${OutputShadersFile}/${File}.spv
        #     COMMENT "Copied ${InputShadersFile}/${File}.spv to ${OutputShadersFile}/${File}.spv"
        #     )
    endforeach()
endforeach()
