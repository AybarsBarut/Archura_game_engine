#pragma once

#include "ConsoleCommand.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <functional>

namespace Archura {

    /**
     * @class DeveloperConsole
     * @brief FPS oyunları için kapsamlı geliştirici konsolu sistemi
     * 
     * Özellikler:
     * - Komut yürütme sistemi
     * - Konsol değişkenleri (CVAR) yönetimi
     * - Komut geçmişi
     * - Kategori bazlı komut organizasyonu
     */
    class DeveloperConsole {
    public:
        static DeveloperConsole& GetInstance() {
            static DeveloperConsole instance;
            return instance;
        }

        // Komut ve değişken kayıt
        void RegisterCommand(const std::shared_ptr<ConsoleCommand>& command);
        void RegisterVariable(const std::shared_ptr<ConsoleVariable>& variable);

        // Komut yürütme
        void ExecuteCommand(const std::string& commandLine);
        void ExecuteCommandWithArgs(const std::string& command, const std::vector<std::string>& args);

        // Konsol değişkeni yönetimi
        std::shared_ptr<ConsoleVariable> GetVariable(const std::string& name) const;
        void SetVariableValue(const std::string& name, const std::string& value);
        std::string GetVariableValue(const std::string& name) const;

        // Hile modu kontrolü
        void SetCheatsEnabled(bool enabled) { m_cheatsEnabled = enabled; }
        bool AreCheatsEnabled() const { return m_cheatsEnabled; }

        // Komut bilgisi
        std::shared_ptr<ConsoleCommand> GetCommand(const std::string& name) const;
        std::vector<std::string> GetCommandNames() const;
        std::vector<std::string> GetVariableNames() const;

        // Konsol yardımı
        void PrintHelp(const std::string& commandName) const;
        void PrintAllCommands() const;
        void PrintAllVariables() const;

        // Komut geçmişi
        void AddToHistory(const std::string& command);
        const std::vector<std::string>& GetCommandHistory() const { return m_commandHistory; }
        void ClearHistory() { m_commandHistory.clear(); }

        // Print Callback
        using PrintCallback = std::function<void(const std::string&)>;
        void AddPrintCallback(PrintCallback callback) { m_printCallbacks.push_back(callback); }
        
        // Helper to print to callback + stdout
        void Print(const std::string& message) const;

        // Kaydet ve yükle
        void SaveConfig(const std::string& filename);
        void LoadConfig(const std::string& filename);

    private:
        DeveloperConsole();
        void RegisterBuiltinCommands();
        void RegisterBuiltinVariables();

        std::unordered_map<std::string, std::shared_ptr<ConsoleCommand>> m_commands;
        std::unordered_map<std::string, std::shared_ptr<ConsoleVariable>> m_variables;
        std::vector<std::string> m_commandHistory;
        bool m_cheatsEnabled = false;
        std::vector<PrintCallback> m_printCallbacks;

        // Maksimum geçmiş
        static constexpr size_t MAX_HISTORY = 256;
    };

} // namespace Archura
