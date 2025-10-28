cmake_minimum_required (VERSION 3.12)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project(FluidSimulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(VENDOR_DIR ${SRC_DIR}/vendor)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(LIB_DIR ${CMAKE_SOURCE_DIR}/libs)
set(SHADER_DIR ${CMAKE_SOURCE_DIR}/shaders)

add_executable(${PROJECT_NAME} 
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/shader.cpp
    ${SRC_DIR}/fluidsolver.cpp
    ${VENDOR_DIR}/imgui.cpp
    ${VENDOR_DIR}/imgui_draw.cpp 
    ${VENDOR_DIR}/imgui_impl_glfw.cpp
    ${VENDOR_DIR}/imgui_impl_opengl3.cpp
    ${VENDOR_DIR}/imgui_tables.cpp
    ${VENDOR_DIR}/imgui_widgets.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${SRC_DIR} ${VENDOR_DIR} ${INCLUDE_DIR})

target_link_directories(${PROJECT_NAME} PRIVATE ${LIB_DIR})

target_compile_definitions(${PROJECT_NAME} PRIVATE GLEW_STATIC)

target_link_libraries(${PROJECT_NAME} PRIVATE glew32s.lib glfw3.lib opengl32.lib)

target_compile_definitions(${PROJECT_NAME} PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLEW)

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

foreach(SHADER_FILE ${SHADER_DIR}/vertexshader.glsl ${SHADER_DIR}/fragmentshader.glsl)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SHADER_FILE}
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endforeach()