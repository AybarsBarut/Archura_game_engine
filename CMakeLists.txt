cmake_minimum_required(VERSION 3.15)


if(POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
endif()

project(ArchuraEngine VERSION 1.0.0 LANGUAGES CXX C)

# C++ Standard
# Define APIENTRY globally to fix Windows SDK syntax errors
add_compile_definitions(APIENTRY=__stdcall)
add_compile_definitions(_APIENTRY_DEFINED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Force MSVC Runtime to be Dynamic (DLL) to avoid LIBCMT conflicts
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

# Compiler-specific optimizations
if(MSVC)
    add_compile_options(
        $<$<CONFIG:Release>:/O2>
        $<$<CONFIG:Release>:/Ob2>
        $<$<CONFIG:Release>:/Oi>
        $<$<CONFIG:Release>:/Ot>
        $<$<CONFIG:Release>:/GL>
        $<$<CONFIG:Debug>:/Od>
        $<$<CONFIG:Debug>:/Zi>
    )
    add_link_options($<$<CONFIG:Release>:/LTCG>)
    # Ignore LIBCMT to prevent LNK4098 warning when using /MD
    add_link_options("/NODEFAULTLIB:LIBCMT")
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    add_compile_options(/W4 /MP)
else()
    add_compile_options(-O3 -march=native -ffast-math -flto)
endif()

# Output Directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Dependencies
find_package(OpenGL REQUIRED)

# GLFW - Option to use pre-built or build from source
option(USE_PREBUILT_GLFW "Use pre-built GLFW library instead of building from source" OFF)

if(USE_PREBUILT_GLFW)
    # Use pre-built GLFW (avoids Windows header conflicts during build)
    # User should place pre-built GLFW in external/glfw-prebuilt/
    set(GLFW_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/external/glfw-prebuilt/include")
    set(GLFW_LIBRARY "${CMAKE_SOURCE_DIR}/external/glfw-prebuilt/lib/glfw3.lib")
    
    add_library(glfw STATIC IMPORTED)
    set_target_properties(glfw PROPERTIES
        IMPORTED_LOCATION "${GLFW_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${GLFW_INCLUDE_DIR}"
    )
    
    message(STATUS "Using pre-built GLFW from: ${GLFW_INCLUDE_DIR}")
else()
    # Build GLFW from source
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    add_subdirectory(external/glfw)
    # Prevent Windows header conflicts in GLFW
    target_compile_definitions(glfw PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
    # Prevent PDB locking issues in parallel builds
    target_compile_options(glfw PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/FS>)
    
    message(STATUS "Building GLFW from source")
endif()

# GLAD - Use wrapper to ensure proper Windows header order
add_library(glad src/glad_wrapper.c)
target_include_directories(glad PUBLIC external/glad/include)

# ImGui - Use wrappers to ensure proper Windows header order
set(IMGUI_DIR external/imgui)
add_library(imgui STATIC
    src/imgui_wrapper.cpp
    src/imgui_demo_wrapper.cpp
    src/imgui_draw_wrapper.cpp
    src/imgui_tables_wrapper.cpp
    src/imgui_widgets_wrapper.cpp
    src/imgui_impl_glfw_wrapper.cpp
    src/imgui_impl_opengl3_wrapper.cpp
)
target_include_directories(imgui PUBLIC 
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    external/glad/include
)
target_link_libraries(imgui PUBLIC glfw OpenGL::GL)

# Prevent GLFW from defining APIENTRY (we handle it in wrappers)
target_compile_definitions(imgui PRIVATE GLFW_INCLUDE_NONE)
# Force define APIENTRY to avoid Windows SDK syntax errors
target_compile_definitions(imgui PRIVATE APIENTRY=__stdcall)
target_compile_definitions(glad PRIVATE APIENTRY=__stdcall)
# This line is no longer needed as wrappers handle it
# target_compile_definitions(imgui PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)

# ufbx (FBX Loader)
add_library(ufbx external/ufbx/ufbx.c)
target_include_directories(ufbx PUBLIC external/ufbx)

# Engine Sources
file(GLOB_RECURSE ENGINE_SOURCES 
    "src/core/*.cpp"
    "src/rendering/*.cpp"
    "src/ecs/*.cpp"

    "src/input/*.cpp"
    "src/game/*.cpp"
    "src/editor/*.cpp"
    "src/network/*.cpp"
)

file(GLOB_RECURSE ENGINE_HEADERS 
    "src/core/*.h"
    "src/rendering/*.h"
    "src/ecs/*.h"

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


