#pragma once

#include <string>
#include <vector>
#include <imgui.h>

namespace Archura {

    class DevConsole {
    public:
        static DevConsole& Get();

        void Init();
        void Toggle();
        void Render();
        void Log(const std::string& message);

        bool IsOpen() const { return m_IsOpen; }
        bool IsDevMode() const { return m_DevMode; }
        void SetDevMode(bool enabled) { m_DevMode = enabled; }

    private:
        bool m_IsOpen = false;
        bool m_DevMode = false;
        char m_InputBuf[256] = "";
        std::vector<std::string> m_Logs;
        bool m_ScrollToBottom = false;
    };

} // namespace Archura
