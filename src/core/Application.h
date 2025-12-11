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
        std::unique_ptr<Window> m_Window;
        std::unique_ptr<class ImGuiLayer> m_ImGuiLayer;
        bool m_Running = true;
        
        // Performance Monitoring
        double m_LastFrameTime = 0.0;
    };

}
