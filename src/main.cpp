#include "core/Application.h"
#include "network/NetworkManager.h"
#include <iostream>
#include <cstdio>

// Harici GPU Seçimi (NVIDIA / AMD)
extern "C" {
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int main(int argc, char** argv) {
    // Check for console argument
    bool showConsole = false;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--console") == 0 || strcmp(argv[i], "-console") == 0) {
            showConsole = true;
            break;
        }
    }

    if (showConsole) {
        AllocConsole();
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
        std::cout << "Debug Console Allocated\n";
    }

    auto app = std::make_unique<Archura::Application>();
    app->Run();
    return 0;
}
