#pragma once

#include "Window.h"

namespace Archura {

    class ImGuiLayer {
    public:
        ImGuiLayer();
        ~ImGuiLayer();

        void Init(Window* window);
        void Shutdown();

        void BeginFrame();
        void EndFrame();

        void BlockEvents(bool block) { m_BlockEvents = block; }

    private:
        bool m_BlockEvents = true;
    };

}
