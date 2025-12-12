#include "PauseMenu.h"
#include <imgui.h>
#include <string>
#include <iostream>
#include <vector>
#include "../core/Application.h"
#include "../game/FPSController.h"
#include "../core/Window.h"
#include "../network/NetworkManager.h"

namespace Archura {

const char* PauseMenu::GetKeyName(int keycode) {
    switch (keycode) {
        case 32: return "SPACE";
        case 39: return "APOSTROPHE";
        case 44: return "COMMA";
        case 45: return "MINUS";
        case 46: return "PERIOD";
        case 47: return "SLASH";
        case 48: return "0";
        case 49: return "1";
        case 50: return "2";
        case 51: return "3";
        case 52: return "4";
        case 53: return "5";
        case 54: return "6";
        case 55: return "7";
        case 56: return "8";
        case 57: return "9";
        case 59: return "SEMICOLON";
        case 61: return "EQUAL";
        case 65: return "A";
        case 66: return "B";
        case 67: return "C";
        case 68: return "D";
        case 69: return "E";
        case 70: return "F";
        case 71: return "G";
        case 72: return "H";
        case 73: return "I";
        case 74: return "J";
        case 75: return "K";
        case 76: return "L";
        case 77: return "M";
        case 78: return "N";
        case 79: return "O";
        case 80: return "P";
        case 81: return "Q";
        case 82: return "R";
        case 83: return "S";
        case 84: return "T";
        case 85: return "U";
        case 86: return "V";
        case 87: return "W";
        case 88: return "X";
        case 89: return "Y";
        case 90: return "Z";
        case 91: return "LEFT BRACKET";
        case 92: return "BACKSLASH";
        case 93: return "RIGHT BRACKET";
        case 96: return "GRAVE ACCENT";
        case 256: return "ESCAPE";
        case 257: return "ENTER";
        case 258: return "TAB";
        case 259: return "BACKSPACE";
        case 260: return "INSERT";
        case 261: return "DELETE";
        case 262: return "RIGHT";
        case 263: return "LEFT";
        case 264: return "DOWN";
        case 265: return "UP";
        case 266: return "PAGE UP";
        case 267: return "PAGE DOWN";
        case 268: return "HOME";
        case 269: return "END";
        case 280: return "CAPS LOCK";
        case 281: return "SCROLL LOCK";
        case 282: return "NUM LOCK";
        case 283: return "PRINT SCREEN";
        case 284: return "PAUSE";
        case 290: return "F1";
        case 291: return "F2";
        case 292: return "F3";
        case 293: return "F4";
        case 294: return "F5";
        case 295: return "F6";
        case 296: return "F7";
        case 297: return "F8";
        case 298: return "F9";
        case 299: return "F10";
        case 300: return "F11";
        case 301: return "F12";
        case 340: return "LEFT SHIFT";
        case 341: return "LEFT CONTROL";
        case 342: return "LEFT ALT";
        case 344: return "RIGHT SHIFT";
        default: return "UNKNOWN";
    }
}

    void PauseMenu::Render(bool& isPaused, FPSController& controller, Window& window) {
        if (!isPaused) {
            m_CurrentState = MenuState::Main; // Reset to main when closed
            return;
        }

        ImGuiIO& io = ImGui::GetIO();
        
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.85f));

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | 
                                 ImGuiWindowFlags_NoMove | 
                                 ImGuiWindowFlags_NoResize | 
                                 ImGuiWindowFlags_NoSavedSettings | 
                                 ImGuiWindowFlags_NoBringToFrontOnFocus;

        if (ImGui::Begin("PauseMenu", nullptr, flags)) {
            
            // Common Center Layout Calculation
            float centerX = io.DisplaySize.x * 0.5f;
            float centerY = io.DisplaySize.y * 0.5f;
            float btnWidth = 300.0f;
            float spacing = 20.0f;

            // Title
            ImGui::SetWindowFontScale(3.0f);
            const char* title = "ARCHURA PAUSED";
            if (m_CurrentState == MenuState::Options) title = "OPTIONS";
            if (m_CurrentState == MenuState::Keybinds) title = "CONTROLS";

            float titleW = ImGui::CalcTextSize(title).x;
            ImGui::SetCursorPos(ImVec2(centerX - titleW * 0.5f, centerY - 250.0f));
            ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), title);
            ImGui::SetWindowFontScale(1.0f);

            // Menu Content
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
            
            // Layout Container - Calculate height approximate to center vertically
            float contentHeight = 400.0f; // Approximate
            ImGui::SetCursorPos(ImVec2(centerX - btnWidth * 0.5f, centerY - contentHeight * 0.4f));
            ImGui::BeginGroup();

            switch (m_CurrentState) {
                case MenuState::Main:
                    RenderMainMenu(isPaused);
                    break;
                case MenuState::Options:
                    RenderOptions(controller, window);
                    break;
                case MenuState::Keybinds:
                    RenderKeybinds(controller);
                    break;
            }

            ImGui::EndGroup();
            ImGui::PopStyleVar();
        }
        ImGui::End();
        ImGui::PopStyleColor();
    }

    void PauseMenu::RenderMainMenu(bool& isPaused) {
        float btnWidth = 300.0f;
        float btnHeight = 50.0f;
        float spacing = 15.0f;

        if (ImGui::Button("RESUME GAME", ImVec2(btnWidth, btnHeight))) {
            isPaused = false;
        }

        ImGui::Dummy(ImVec2(0, spacing));

        if (ImGui::Button("OPTIONS", ImVec2(btnWidth, btnHeight))) {
            m_CurrentState = MenuState::Options;
        }

        ImGui::Dummy(ImVec2(0, spacing));

        if (ImGui::Button("QUIT TO DESKTOP", ImVec2(btnWidth, btnHeight))) {
            Application::Get().Quit();
        }
    }

    void PauseMenu::RenderOptions(FPSController& controller, Window& window) {
        float width = 300.0f;
        
        // --- SENSITIVITY ---
        ImGui::Text("Mouse Sensitivity");
        ImGui::SetNextItemWidth(width);
        float sens = controller.GetMouseSensitivity();
        if (ImGui::SliderFloat("##sens", &sens, 0.01f, 1.0f, "%.2f")) {
            controller.SetMouseSensitivity(sens);
        }
        ImGui::Dummy(ImVec2(0, 10));

        // --- RESOLUTION ---
        ImGui::Text("Resolution");
        const char* resolutions[] = { "1280x720", "1600x900", "1920x1080", "2560x1440" };
        static int currentRes = 2; // Default to 1920x1080 check?
        
        // Sync combo with actual window size
        if (window.GetWidth() == 1280) currentRes = 0;
        else if (window.GetWidth() == 1600) currentRes = 1;
        else if (window.GetWidth() == 1920) currentRes = 2;
        else if (window.GetWidth() == 2560) currentRes = 3;

        ImGui::SetNextItemWidth(width);
        if (ImGui::Combo("##res", &currentRes, resolutions, IM_ARRAYSIZE(resolutions))) {
            switch (currentRes) {
                case 0: window.SetResolution(1280, 720); break;
                case 1: window.SetResolution(1600, 900); break;
                case 2: window.SetResolution(1920, 1080); break;
                case 3: window.SetResolution(2560, 1440); break;
            }
        }
        ImGui::SameLine();
        bool fullscreen = window.IsFullscreen();
        if (ImGui::Checkbox("Fullscreen", &fullscreen)) {
            window.SetFullscreen(fullscreen);
        }

        ImGui::Dummy(ImVec2(0, 10));

        // --- NETWORK (Game Host) ---
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Game Host / Network");
        
        static int port = 27015;
        static char ip[32] = "127.0.0.1";

        ImGui::Text("Port");
        ImGui::SetNextItemWidth(width);
        ImGui::InputInt("##port", &port, 0); // STEP=0 removes +/- buttons for cleaner look, or keep them? 
                                             // Default is 1, 100. Let's keep formatted input or default. 
                                             // User asked to shorten. InputInt defaults with buttons.
        
        if (ImGui::Button("HOST GAME", ImVec2(width, 30))) {
            NetworkManager::Get().StartServer(port);
        }

        ImGui::Text("IP Address");
        ImGui::SetNextItemWidth(width);
        ImGui::InputText("##ip", ip, 32);

        if (ImGui::Button("JOIN GAME", ImVec2(width, 30))) {
            NetworkManager::Get().Connect(ip, port);
        }
        
        // Status
        if (NetworkManager::Get().IsServer()) ImGui::TextColored(ImVec4(0,1,0,1), "Hosting on port %d", port);
        if (NetworkManager::Get().IsConnected() && !NetworkManager::Get().IsServer()) ImGui::TextColored(ImVec4(0,1,0,1), "Connected to server");

        ImGui::Dummy(ImVec2(0, 20));
        
        // --- BUTTONS ---
        if (ImGui::Button("KEY BINDINGS", ImVec2(width, 40))) {
            m_CurrentState = MenuState::Keybinds;
        }

        ImGui::Dummy(ImVec2(0, 10));

        if (ImGui::Button("BACK", ImVec2(width, 40))) {
            m_CurrentState = MenuState::Main;
        }
    }

    void PauseMenu::RenderKeybinds(FPSController& controller) {
        ImGui::Text("Key Bindings Configuration");
        ImGui::Dummy(ImVec2(0, 10));

        KeyBindings& binds = controller.GetKeyBindings();

        float labelWidth = 120.0f;
        
        auto RenderBind = [&](const char* label, int key) {
            ImGui::Text(label); 
            ImGui::SameLine(labelWidth);
            ImGui::Button(GetKeyName(key), ImVec2(180, 0));
        };

        RenderBind("Forward:", binds.forward);
        RenderBind("Backward:", binds.backward);
        RenderBind("Left:", binds.left);
        RenderBind("Right:", binds.right);
        RenderBind("Jump:", binds.jump);
        RenderBind("Sprint:", binds.sprint);

        ImGui::Dummy(ImVec2(0, 30));

        if (ImGui::Button("BACK", ImVec2(300, 40))) {
            m_CurrentState = MenuState::Options;
        }
    }

}
