cmake_minimum_required(VERSION 3.15)

# Suppress policy warnings
if(POLICY CMP0091)
  cmake_policy(SET CMP0091 OLD)
endif()
if(POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
endif()

project(ArchuraEngine VERSION 1.0.0 LANGUAGES CXX)

# C++ standardı
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Output dizinleri
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Bağımlılıkları bul
find_package(OpenGL REQUIRED)

# Policy settings for external projects
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

# GLFW - Platform independent window ve input
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

# GLM - Header-only library
# add_subdirectory(external/glm)  # GLM modern CMake ile sorun çıkarıyor
# GLM header-only olduğu için sadece include yeterli

# GLAD - OpenGL fonksiyon yükleyici
add_library(glad external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

# ImGui - Editor UI
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

# Engine kaynak dosyaları
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

# Ana executable
add_executable(${PROJECT_NAME} 
    src/main.cpp
    ${ENGINE_SOURCES}
    ${ENGINE_HEADERS}
)

# Include dizinleri
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/external/stb
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glm
)

# Kütüphaneleri bağla
target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    glfw
    glad
    imgui
    imgui
    ws2_32
    winmm
)

# Windows için özel ayarlar
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
    # Console penceresi istenmiyorsa:
    # set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

# Asset dosyalarını çalıştırılabilir yanına kopyala
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)

# Compiler uyarıları
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()
