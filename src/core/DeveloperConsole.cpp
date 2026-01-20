#include "DeveloperConsole.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <fstream>


namespace Archura {

    DeveloperConsole::DeveloperConsole() {
        RegisterBuiltinCommands();
        RegisterBuiltinVariables();
    }

    void DeveloperConsole::RegisterCommand(const std::shared_ptr<ConsoleCommand>& command) {
        if (command) {
            m_commands[command->GetName()] = command;
        }
    }

    void DeveloperConsole::RegisterVariable(const std::shared_ptr<ConsoleVariable>& variable) {
        if (variable) {
            m_variables[variable->GetName()] = variable;
        }
    }

    void DeveloperConsole::ExecuteCommand(const std::string& commandLine) {
        if (commandLine.empty()) return;

        std::istringstream iss(commandLine);
        std::string command;
        iss >> command;

        std::vector<std::string> args;
        std::string arg;
        while (iss >> arg) {
            args.push_back(arg);
        }

        ExecuteCommandWithArgs(command, args);
    }

    void DeveloperConsole::Print(const std::string& message) const {
        std::cout << message << std::endl;
        for (const auto& callback : m_printCallbacks) {
            callback(message);
        }
    }

    void DeveloperConsole::ExecuteCommandWithArgs(const std::string& command, const std::vector<std::string>& args) {
        // Komut geçmişine ekle
        std::string fullCommand = command;
        for (const auto& arg : args) {
            fullCommand += " " + arg;
        }
        AddToHistory(fullCommand);

        // Komutu bulup çalıştır
        auto it = m_commands.find(command);
        if (it != m_commands.end()) {
            auto cmdPtr = it->second;
            if (cmdPtr->RequiresCheats() && !m_cheatsEnabled) {
                Print("[Console] '" + command + "' requires sv_cheats 1");
                return;
            }
            cmdPtr->Execute(args);
            return;
        }

        // CVAR olarak deneyelim
        if (args.empty()) {
            // Sadece değişkeni oku
            auto varIt = m_variables.find(command);
            if (varIt != m_variables.end()) {
                auto var = varIt->second;
                Print(command + " = \"" + var->GetValue() + "\"");
                return;
            }
        }
        else if (args.size() == 1) {
            // Değişkene yeni değer ata
            auto varIt = m_variables.find(command);
            if (varIt != m_variables.end()) {
                auto var = varIt->second;
                if (var->RequiresCheats() && !m_cheatsEnabled) {
                    Print("[Console] '" + command + "' requires sv_cheats 1");
                    return;
                }
                var->SetValue(args[0]);
                Print(command + " = \"" + args[0] + "\"");
                return;
            }
        }

        Print("[Console] Unknown command: " + command);
        Print("[Console] Type 'help' for more information");
    }

    std::shared_ptr<ConsoleVariable> DeveloperConsole::GetVariable(const std::string& name) const {
        auto it = m_variables.find(name);
        return (it != m_variables.end()) ? it->second : nullptr;
    }

    void DeveloperConsole::SetVariableValue(const std::string& name, const std::string& value) {
        auto var = GetVariable(name);
        if (var) {
            var->SetValue(value);
        }
    }

    std::string DeveloperConsole::GetVariableValue(const std::string& name) const {
        auto var = GetVariable(name);
        return var ? var->GetValue() : "";
    }

    std::shared_ptr<ConsoleCommand> DeveloperConsole::GetCommand(const std::string& name) const {
        auto it = m_commands.find(name);
        return (it != m_commands.end()) ? it->second : nullptr;
    }

    std::vector<std::string> DeveloperConsole::GetCommandNames() const {
        std::vector<std::string> names;
        for (const auto& pair : m_commands) {
            names.push_back(pair.first);
        }
        std::sort(names.begin(), names.end());
        return names;
    }

    std::vector<std::string> DeveloperConsole::GetVariableNames() const {
        std::vector<std::string> names;
        for (const auto& pair : m_variables) {
            names.push_back(pair.first);
        }
        std::sort(names.begin(), names.end());
        return names;
    }

    void DeveloperConsole::PrintHelp(const std::string& commandName) const {
        std::stringstream ss;
        ss << "\n=== Help ===\n";
        ss << "Command: " << commandName << "\n";

        auto cmd = GetCommand(commandName);
        if (cmd) {
            ss << "Description: " << cmd->GetDescription() << "\n";
            if (cmd->RequiresCheats()) {
                ss << "Note: Requires sv_cheats 1\n";
            }
        }
        else {
            auto var = GetVariable(commandName);
            if (var) {
                ss << "Type: ";
                switch (var->GetType()) {
                    case ConsoleVariable::Type::Float: ss << "Float"; break;
                    case ConsoleVariable::Type::Int: ss << "Int"; break;
                    case ConsoleVariable::Type::String: ss << "String"; break;
                    case ConsoleVariable::Type::Bool: ss << "Bool (0/1)"; break;
                }
                ss << "\n";
                ss << "Default: " << var->GetDefaultValue() << "\n";
                ss << "Current: " << var->GetValue() << "\n";
                ss << "Description: " << var->GetDescription() << "\n";
                if (var->RequiresCheats()) {
                    ss << "Note: Requires sv_cheats 1\n";
                }
            }
            else {
                ss << "Unknown command or variable\n";
            }
        }
        ss << "=============\n";
        Print(ss.str());
    }

    void DeveloperConsole::PrintAllCommands() const {
        std::stringstream ss;
        ss << "\n=== Available Commands ===\n";
        auto names = GetCommandNames();
        for (const auto& name : names) {
            auto cmd = GetCommand(name);
            if (cmd) {
                ss << name << " - " << cmd->GetDescription();
                if (cmd->RequiresCheats()) {
                     ss << " [CHEATS]";
                }
                ss << "\n";
            }
        }
        ss << "===========================\n";
        Print(ss.str());
    }

    void DeveloperConsole::PrintAllVariables() const {
        std::stringstream ss;
        ss << "\n=== Available Variables (CVARs) ===\n";
        auto names = GetVariableNames();
        for (const auto& name : names) {
            auto var = GetVariable(name);
            if (var) {
                ss << name << " = \"" << var->GetValue() << "\"";
                if (var->RequiresCheats()) {
                    ss << " [CHEATS]";
                }
                ss << "\n";
            }
        }
        ss << "=====================================\n";
        Print(ss.str());
    }

    void DeveloperConsole::AddToHistory(const std::string& command) {
        m_commandHistory.push_back(command);
        if (m_commandHistory.size() > MAX_HISTORY) {
            m_commandHistory.erase(m_commandHistory.begin());
        }
    }

    void DeveloperConsole::SaveConfig(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cout << "[Console] Failed to save config to " << filename << std::endl;
            return;
        }

        // Header
        file << "// Archura FPS Engine - Configuration File\n";
        file << "// Auto-generated config\n\n";

        // CVARs
        file << "// === Console Variables ===\n";
        auto varNames = GetVariableNames();
        for (const auto& name : varNames) {
            auto var = GetVariable(name);
            if (var) {
                file << name << " \"" << var->GetValue() << "\"\n";
            }
        }

        file.close();
        std::cout << "[Console] Config saved to " << filename << std::endl;
    }

    void DeveloperConsole::LoadConfig(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "[Console] Failed to load config from " << filename << std::endl;
            return;
        }

        std::string line;
        int lineCount = 0;
        while (std::getline(file, line)) {
            // Yorum satırları ve boş satırları atla
            if (line.empty() || line[0] == '/' || line[0] == ';') {
                continue;
            }

            std::istringstream iss(line);
            std::string varName, varValue;
            if (iss >> varName >> varValue) {
                // Tırnak işaretlerini çıkar
                if (!varValue.empty() && varValue.front() == '"' && varValue.back() == '"') {
                    varValue = varValue.substr(1, varValue.length() - 2);
                }
                SetVariableValue(varName, varValue);
                lineCount++;
            }
        }

        file.close();
        std::cout << "[Console] Loaded " << lineCount << " config entries from " << filename << std::endl;
    }

    void DeveloperConsole::RegisterBuiltinCommands() {
        // === Sistem Komutları ===
        RegisterCommand(std::make_shared<ConsoleCommand>(
            "help",
            "Show help for a command or variable",
            [this](const std::vector<std::string>& args) {
                if (args.empty()) {
                    PrintAllCommands();
                }
                else {
                    PrintHelp(args[0]);
                }
            }
        ));

        RegisterCommand(std::make_shared<ConsoleCommand>(
            "cmdlist",
            "List all available commands",
            [this](const std::vector<std::string>& args) {
                PrintAllCommands();
            }
        ));

        RegisterCommand(std::make_shared<ConsoleCommand>(
            "cvarlist",
            "List all available variables (CVARs)",
            [this](const std::vector<std::string>& args) {
                PrintAllVariables();
            }
        ));

        RegisterCommand(std::make_shared<ConsoleCommand>(
            "clear",
            "Clear the console",
            [](const std::vector<std::string>& args) {
                system("cls"); // Windows
            }
        ));

        RegisterCommand(std::make_shared<ConsoleCommand>(
            "echo",
            "Echo text to console",
            [](const std::vector<std::string>& args) {
                for (size_t i = 0; i < args.size(); ++i) {
                    if (i > 0) std::cout << " ";
                    std::cout << args[i];
                }
                std::cout << "\n";
            }
        ));

        RegisterCommand(std::make_shared<ConsoleCommand>(
            "quit",
            "Exit the game",
            [](const std::vector<std::string>& args) {
                std::cout << "[Console] Quitting...\n";
                exit(0);
            }
        ));

        RegisterCommand(std::make_shared<ConsoleCommand>(
            "exit",
            "Exit the game",
            [](const std::vector<std::string>& args) {
                std::cout << "[Console] Quitting...\n";
                exit(0);
            }
        ));

        RegisterCommand(std::make_shared<ConsoleCommand>(
            "history",
            "Show command history",
            [this](const std::vector<std::string>& args) {
                const auto& history = GetCommandHistory();
                std::cout << "\n=== Command History ===\n";
                for (size_t i = 0; i < history.size(); ++i) {
                    std::cout << (i + 1) << ": " << history[i] << "\n";
                }
                std::cout << "=======================\n\n";
            }
        ));

        RegisterCommand(std::make_shared<ConsoleCommand>(
            "save_config",
            "Save current configuration to file",
            [this](const std::vector<std::string>& args) {
                std::string filename = args.empty() ? "game.cfg" : args[0];
                SaveConfig(filename);
            }
        ));

        RegisterCommand(std::make_shared<ConsoleCommand>(
            "load_config",
            "Load configuration from file",
            [this](const std::vector<std::string>& args) {
                std::string filename = args.empty() ? "game.cfg" : args[0];
                LoadConfig(filename);
            }
        ));

        RegisterCommand(std::make_shared<ConsoleCommand>(
            "reset_config",
            "Reset all variables to defaults",
            [this](const std::vector<std::string>& args) {
                auto varNames = GetVariableNames();
                for (const auto& name : varNames) {
                    auto var = GetVariable(name);
                    if (var) {
                        var->ResetToDefault();
                    }
                }
                std::cout << "[Console] All variables reset to default\n";
            }
        ));
    }

    void DeveloperConsole::RegisterBuiltinVariables() {
        // === Grafik Ayarları (r_) ===
        RegisterVariable(std::make_shared<ConsoleVariable>(
            "r_fps_max",
            "144",
            ConsoleVariable::Type::Int,
            "Maximum FPS limit (0 = unlimited)"
        ));

        RegisterVariable(std::make_shared<ConsoleVariable>(
            "r_fps_display",
            "0",
            ConsoleVariable::Type::Bool,
            "Display FPS counter"
        ));

        RegisterVariable(std::make_shared<ConsoleVariable>(
            "r_frametime_display",
            "0",
            ConsoleVariable::Type::Bool,
            "Display frame time in milliseconds"
        ));

        RegisterVariable(std::make_shared<ConsoleVariable>(
            "r_vsync",
            "1",
            ConsoleVariable::Type::Bool,
            "Enable vertical sync"
        ));

        RegisterVariable(std::make_shared<ConsoleVariable>(
            "r_resolution",
            "1920 1080",
            ConsoleVariable::Type::String,
            "Screen resolution (width height)"
        ));

        RegisterVariable(std::make_shared<ConsoleVariable>(
            "r_fullscreen",
            "1",
            ConsoleVariable::Type::Bool,
            "Enable fullscreen mode"
        ));

        RegisterVariable(std::make_shared<ConsoleVariable>(
            "r_fov",
            "90",
            ConsoleVariable::Type::Float,
            "Field of View in degrees"
        ));

        RegisterVariable(std::make_shared<ConsoleVariable>(
            "r_texture_quality",
            "3",
            ConsoleVariable::Type::Int,
            "Texture quality (0=Low, 4=Ultra)"
        ));

        RegisterVariable(std::make_shared<ConsoleVariable>(
            "r_shadow_quality",
            "2",
            ConsoleVariable::Type::Int,
            "Shadow quality (0=Off, 4=Ultra)"
        ));

        RegisterVariable(std::make_shared<ConsoleVariable>(
            "r_draw_distance",
            "3000",
            ConsoleVariable::Type::Float,
            "Maximum render distance in meters"
        ));

        RegisterVariable(std::make_shared<ConsoleVariable>(
            "r_aa_mode",
            "1",
            ConsoleVariable::Type::Int,
            "Anti-aliasing mode (0=Off, 1=FXAA, 2=SMAA, 3=TAA)"
        ));

        RegisterVariable(std::make_shared<ConsoleVariable>(
            "r_bloom_enabled",
            "1",
            ConsoleVariable::Type::Bool,
            "Enable bloom effect"
        ));

        // === Ses Ayarları (snd_) ===
        RegisterVariable(std::make_shared<ConsoleVariable>(
            "snd_master_volume",
            "0.8",
            ConsoleVariable::Type::Float,
            "Master volume level (0.0-1.0)"
        ));

        RegisterVariable(std::make_shared<ConsoleVariable>(
            "snd_music_volume",
            "0.5",
            ConsoleVariable::Type::Float,
            "Music volume level"
        ));

        RegisterVariable(std::make_shared<ConsoleVariable>(
            "snd_effects_volume",
            "0.9",
            ConsoleVariable::Type::Float,
            "Effects volume level"
        ));

        RegisterVariable(std::make_shared<ConsoleVariable>(
            "snd_3d_enabled",
            "1",
            ConsoleVariable::Type::Bool,
            "Enable 3D spatial audio"
        ));

        // === Girdi Ayarları (m_) ===
        RegisterVariable(std::make_shared<ConsoleVariable>(
            "m_sensitivity",
            "1.0",
            ConsoleVariable::Type::Float,
            "Mouse sensitivity (0.1-5.0)"
        ));

        RegisterVariable(std::make_shared<ConsoleVariable>(
            "m_rawmouse",
            "1",
            ConsoleVariable::Type::Bool,
            "Use raw mouse input"
        ));

        RegisterVariable(std::make_shared<ConsoleVariable>(
            "m_accel",
            "0.0",
            ConsoleVariable::Type::Float,
            "Mouse acceleration (0.0-1.0)"
        ));

        // === İstemci Ayarları (cl_) ===
        RegisterVariable(std::make_shared<ConsoleVariable>(
            "cl_crosshair_enabled",
            "1",
            ConsoleVariable::Type::Bool,
            "Display crosshair"
        ));

        RegisterVariable(std::make_shared<ConsoleVariable>(
            "cl_crosshair_style",
            "0",
            ConsoleVariable::Type::Int,
            "Crosshair style (0-5)"
        ));

        RegisterVariable(std::make_shared<ConsoleVariable>(
            "cl_crosshair_size",
            "20",
            ConsoleVariable::Type::Int,
            "Crosshair size"
        ));

        RegisterVariable(std::make_shared<ConsoleVariable>(
            "cl_predict",
            "1",
            ConsoleVariable::Type::Bool,
            "Enable client-side prediction"
        ));

        RegisterVariable(std::make_shared<ConsoleVariable>(
            "hud_enabled",
            "1",
            ConsoleVariable::Type::Bool,
            "Display HUD"
        ));

        // === Ağ Ayarları (net_) ===
        RegisterVariable(std::make_shared<ConsoleVariable>(
            "net_rate",
            "128000",
            ConsoleVariable::Type::Int,
            "Network data rate in bytes/sec"
        ));

        RegisterVariable(std::make_shared<ConsoleVariable>(
            "net_cmdrate",
            "60",
            ConsoleVariable::Type::Int,
            "Command send rate (packets/sec)"
        ));

        RegisterVariable(std::make_shared<ConsoleVariable>(
            "net_updaterate",
            "32",
            ConsoleVariable::Type::Int,
            "Server update rate (packets/sec)"
        ));

        RegisterVariable(std::make_shared<ConsoleVariable>(
            "net_interpolate",
            "1",
            ConsoleVariable::Type::Bool,
            "Enable motion interpolation"
        ));

        RegisterVariable(std::make_shared<ConsoleVariable>(
            "net_latency",
            "0",
            ConsoleVariable::Type::Int,
            "Simulated latency in milliseconds (debug)"
        ));

        RegisterVariable(std::make_shared<ConsoleVariable>(
            "net_packetloss",
            "0.0",
            ConsoleVariable::Type::Float,
            "Simulated packet loss percentage (debug)"
        ));

        // === Sistem Ayarları (sys_) ===
        RegisterVariable(std::make_shared<ConsoleVariable>(
            "sys_cpu_usage",
            "0",
            ConsoleVariable::Type::Bool,
            "Display CPU usage percentage"
        ));

        RegisterVariable(std::make_shared<ConsoleVariable>(
            "sys_gpu_usage",
            "0",
            ConsoleVariable::Type::Bool,
            "Display GPU usage percentage"
        ));

        RegisterVariable(std::make_shared<ConsoleVariable>(
            "sys_memory_usage",
            "0",
            ConsoleVariable::Type::Bool,
            "Display memory usage in MB"
        ));

        // === Hile Ayarları (sv_) ===
        RegisterVariable(std::make_shared<ConsoleVariable>(
            "sv_cheats",
            "0",
            ConsoleVariable::Type::Bool,
            "Enable cheat commands [ADMIN ONLY]"
        ));

        RegisterVariable(std::make_shared<ConsoleVariable>(
            "sv_gravity",
            "1.0",
            ConsoleVariable::Type::Float,
            "Gravity multiplier [CHEATS]",
            true
        ));

        RegisterVariable(std::make_shared<ConsoleVariable>(
            "sv_infinite_ammo",
            "0",
            ConsoleVariable::Type::Bool,
            "Infinite ammunition [CHEATS]",
            true
        ));
    }

} // namespace Archura
