#pragma once

#include "Window.h"
#include <memory>

namespace Archura {

    class Application {
    public:
        Application();
        virtual ~Application();

        static Application& Get() { return *s_Instance; }

        void Run();
        void Quit() { m_Running = false; }

        Window& GetWindow() { return *m_Window; }

    private:
        bool Init();
        void OnEvent(); // Event polling placeholder if needed here

    private:
        static Application* s_Instance;
        Window* m_Window; // Reference to Engine's window (owned by Engine)
        std::unique_ptr<class ImGuiLayer> m_ImGuiLayer;
        bool m_Running = true;
        
        // Performance Monitoring
        double m_LastFrameTime = 0.0;
        float m_FPSLimit = 0.0f; 

        // Game State (Moved from local Run scope)
        std::unique_ptr<class FPSController> m_FPSController;
        bool m_DevModeActive = true;
        bool m_IsPaused = false;

    public:
        // Console Command Helpers
        void SetFPSLimit(float limit) { m_FPSLimit = limit; }
        void SetSensitivity(float sens);
        void SetDevMode(bool enabled);
        bool IsDevMode() const { return m_DevModeActive; }
        
        class FPSController* GetFPSController() { return m_FPSController.get(); }
    };

}
