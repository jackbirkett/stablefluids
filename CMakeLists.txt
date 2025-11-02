cmake_minimum_required(VERSION 3.20)
project(FluidSimulator LANGUAGES CXX VERSION 1.0)

# ---- General build settings ----
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ---- Dependencies (auto-fetched) ----
include(FetchContent)

# GLFW -------------------------------------------------------------
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# GLEW -------------------------------------------------------------
FetchContent_Declare(
    glew
    GIT_REPOSITORY https://github.com/nigels-com/glew.git
    GIT_TAG        glew-2.2.0
)
set(ONLY_LIBS ON CACHE BOOL "" FORCE)
set(BUILD_UTILS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glew)

# ---- Sources ----
set(SOURCES
    src/main.cpp
    src/fluidsolver.cpp
    src/shader.cpp
    src/vendor/imgui.cpp
    src/vendor/imgui_demo.cpp
    src/vendor/imgui_draw.cpp
    src/vendor/imgui_impl_glfw.cpp
    src/vendor/imgui_impl_opengl3.cpp
    src/vendor/imgui_tables.cpp
    src/vendor/imgui_widgets.cpp
)

set(HEADERS
    src/fluidsolver.h
    src/shader.h
    src/vendor/imconfig.h
    src/vendor/imgui.h
    src/vendor/imgui_impl_glfw.h
    src/vendor/imgui_impl_opengl3.h
    src/vendor/imgui_impl_opengl3_loader.h
    src/vendor/imgui_internal.h
    src/vendor/imstb_rectpack.h
    src/vendor/imstb_textedit.h
    src/vendor/imstb_truetype.h
)

# ---- Target ----
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_compile_definitions(${PROJECT_NAME} PRIVATE GLEW_STATIC)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

# ---- Link platform-specific libraries ----
if (WIN32)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE
            glfw
            libglew_static
            opengl32
            user32
            gdi32
            shell32
    )
elseif (APPLE)
    find_library(COCOA_LIBRARY Cocoa REQUIRED)
    find_library(IOKIT_LIBRARY IOKit REQUIRED)
    find_library(COREFOUNDATION_LIBRARY CoreFoundation REQUIRED)
    find_library(OPENGL_LIBRARY OpenGL REQUIRED)

    target_link_libraries(${PROJECT_NAME}
        PRIVATE
            glfw
            libglew_static
            ${OPENGL_LIBRARY}
            ${COCOA_LIBRARY}
            ${IOKIT_LIBRARY}
            ${COREFOUNDATION_LIBRARY}
    )
else() # Linux / Unix
    find_package(OpenGL REQUIRED)
    find_package(X11 REQUIRED)

    target_link_libraries(${PROJECT_NAME}
        PRIVATE
            glfw
            libglew_static
            OpenGL::GL
            X11
            pthread
            dl
    )
endif()

# ---- Install targets ----
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Copy shader resources on install
install(DIRECTORY shaders/ DESTINATION bin/shaders)

# ---- Optional: export CMake package for find_package(FluidSimulator) ----
include(CMakePackageConfigHelpers)
install(EXPORT FluidSimulatorTargets
    FILE FluidSimulatorTargets.cmake
    NAMESPACE FluidSimulator::
    DESTINATION lib/cmake/FluidSimulator
)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/FluidSimulatorConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/FluidSimulatorConfigVersion.cmake"
    DESTINATION lib/cmake/FluidSimulator
)
