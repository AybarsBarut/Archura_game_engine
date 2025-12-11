#pragma once

namespace Archura {

    // Forward declarations
    class FPSController;
    class Window;

    class PauseMenu {
    public:
        PauseMenu() = default;
        ~PauseMenu() = default;

        // Updated Render to take dependencies
        void Render(bool& isPaused, FPSController& controller, Window& window);

    private:
        void RenderMainMenu(bool& isPaused);
        void RenderOptions(FPSController& controller, Window& window);
        void RenderKeybinds(FPSController& controller);

        // Helpers
        const char* GetKeyName(int keycode);

        enum class MenuState {
            Main,
            Options,
            Keybinds
        };

        MenuState m_CurrentState = MenuState::Main;
        int m_WaitingForKey = -1; // -1: Not waiting, otherwise index of binding
    };

}
