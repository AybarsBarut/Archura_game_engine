// ImGui OpenGL3 Implementation wrapper
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <windows.h>
#endif

// Define GLFW_INCLUDE_NONE to avoid APIENTRY conflicts from GLFW side
#define GLFW_INCLUDE_NONE
#include "../external/glad/include/glad/glad.h"
#include "../external/imgui/backends/imgui_impl_opengl3.cpp"
