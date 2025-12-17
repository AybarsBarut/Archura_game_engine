#include "core/Application.h"

// Harici GPU Seçimi (NVIDIA / AMD)
extern "C" {
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int main(int argc, char** argv) {
    auto app = new Archura::Application();
    app->Run();
    delete app;
    return 0;
}
