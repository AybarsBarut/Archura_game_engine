#include "DevConsole.h"
#include "CommandRegistry.h"
#include <iostream>

namespace Archura {

    DevConsole& DevConsole::Get() {
        static DevConsole instance;
        return instance;
    }

    void DevConsole::Init() {
        // Register default commands
        CommandRegistry::Get().RegisterCommand("dev", [this](const std::vector<std::string>& args) {
            m_DevMode = !m_DevMode;
            Log(m_DevMode ? "Developer Mode ENABLED" : "Developer Mode DISABLED");
        });

        CommandRegistry::Get().RegisterCommand("godmode", [this](const std::vector<std::string>& args) {
            if (!m_DevMode) { Log("Error: Developer Mode required."); return; }
            Log("God Mode Toggled");
        });
        
        CommandRegistry::Get().RegisterCommand("noclip", [this](const std::vector<std::string>& args) {
            if (!m_DevMode) { Log("Error: Developer Mode required."); return; }
            Log("Noclip Toggled");
        });

        Log("Archura DevConsole Initialized. Type /dev to enable developer mode.");
    }

    void DevConsole::Toggle() {
        m_IsOpen = !m_IsOpen;
    }

    void DevConsole::Log(const std::string& message) {
        m_Logs.push_back(message);
        m_ScrollToBottom = true;
    }

    void DevConsole::Render() {
        if (!m_IsOpen) return;

        ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin("Developer Console", &m_IsOpen)) {
            ImGui::End();
            return;
        }

        // Output area
        const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
        
        for (const auto& log : m_Logs) {
            ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
            if (log.find("Error") != std::string::npos) color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
            else if (log.find("ENABLED") != std::string::npos) color = ImVec4(0.4f, 1.0f, 0.4f, 1.0f);
            
            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted(log.c_str());
            ImGui::PopStyleColor();
        }

        if (m_ScrollToBottom || (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
            ImGui::SetScrollHereY(1.0f);
        m_ScrollToBottom = false;

        ImGui::EndChild();

        // Input area
        ImGui::Separator();
        bool reclaim_focus = false;
        ImGuiInputTextFlags input_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
        
        if (ImGui::InputText("Input", m_InputBuf, IM_ARRAYSIZE(m_InputBuf), input_flags)) {
            std::string cmd = m_InputBuf;
            if (!cmd.empty()) {
                Log("> " + cmd);
                CommandRegistry::Get().Dispatch(cmd);
                m_InputBuf[0] = '\0';
            }
            reclaim_focus = true;
        }

        ImGui::SetItemDefaultFocus();
        if (reclaim_focus)
            ImGui::SetKeyboardFocusHere(-1);

        ImGui::End();
    }

} // namespace Archura
