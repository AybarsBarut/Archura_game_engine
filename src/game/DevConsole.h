#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <imgui.h>

namespace Archura {

    class Input; // Forward declaration

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

        // Binding System
        void Bind(int key, const std::string& command);
        void CheckBinds(Input* input);

    private:
        bool m_IsOpen = false;
        bool m_DevMode = false;
        char m_InputBuf[256] = "";
        std::vector<std::string> m_Logs;
        bool m_ScrollToBottom = false;

        std::unordered_map<int, std::string> m_KeyBinds;
    };

} // namespace Archura
