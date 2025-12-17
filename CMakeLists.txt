cmake_minimum_required(VERSION 3.15)


if(POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
endif()

project(ArchuraEngine VERSION 1.0.0 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Force MSVC Runtime to be Dynamic (DLL) to avoid LIBCMT conflicts
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

# Optimization Flags (High Performance)
if(MSVC)
    add_compile_options(
        $<$<CONFIG:Release>:/O2>
        $<$<CONFIG:Release>:/Oi>
        $<$<CONFIG:Release>:/Ot>
        $<$<CONFIG:Release>:/Oy>
        $<$<CONFIG:Release>:/GL>
        $<$<CONFIG:Release>:/fp:fast>
    )
    add_link_options($<$<CONFIG:Release>:/LTCG>)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-O3 -march=native -ffast-math -flto)
endif()

# Output Directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Dependencies
find_package(OpenGL REQUIRED)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

# GLAD
add_library(glad external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

# ImGui
set(IMGUI_DIR external/imgui)
add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC 
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)
target_link_libraries(imgui PUBLIC glfw OpenGL::GL)

# ufbx (FBX Loader)
add_library(ufbx external/ufbx/ufbx.c)
target_include_directories(ufbx PUBLIC external/ufbx)

# Engine Sources
file(GLOB_RECURSE ENGINE_SOURCES 
    "src/core/*.cpp"
    "src/rendering/*.cpp"
    "src/ecs/*.cpp"
    "src/ecs_new/*.cpp"
    "src/input/*.cpp"
    "src/game/*.cpp"
    "src/editor/*.cpp"
    "src/network/*.cpp"
)

file(GLOB_RECURSE ENGINE_HEADERS 
    "src/core/*.h"
    "src/rendering/*.h"
    "src/ecs/*.h"
    "src/ecs_new/*.h"
    "src/input/*.h"
    "src/game/*.h"
    "src/editor/*.h"
    "src/network/*.h"
)

# Main Executable
add_executable(${PROJECT_NAME} 
    src/main.cpp
    ${ENGINE_SOURCES}
    ${ENGINE_HEADERS}
)

# Include Directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/external/stb
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glm
)

# Linking
target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    glfw
    glad
    imgui
    ufbx
    ws2_32
    winmm
)

# Asset Copying
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)

# Launcher
add_executable(ArchuraLauncher src/launcher/Launcher.cpp)
target_link_libraries(ArchuraLauncher PRIVATE ws2_32)
set_target_properties(ArchuraLauncher PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
)
