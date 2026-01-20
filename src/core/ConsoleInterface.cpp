#include "ConsoleInterface.h"
#include "DeveloperConsole.h"
#include "../game/FPSConsoleCommands.h"
#include <iostream>
#include <sstream>

namespace Archura {

    void ConsoleInterface::PrintWelcomeMessage() {
        std::cout << "\n";
        std::cout << "╔════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                  ARCHURA FPS ENGINE CONSOLE                     ║\n";
        std::cout << "║                       Version " << VERSION << "                              ║\n";
        std::cout << "║                                                                ║\n";
        std::cout << "║  Professional Game Development Console                         ║\n";
        std::cout << "║  AAA Quality FPS Game Engine                                    ║\n";
        std::cout << "╚════════════════════════════════════════════════════════════════╝\n";
        std::cout << "\n";
        std::cout << "Type 'help' for command help\n";
        std::cout << "Type 'cmdlist' to see all commands\n";
        std::cout << "Type 'cvarlist' to see all variables\n";
        std::cout << "Type 'quit' to exit\n";
        std::cout << "\n";
    }

    bool ConsoleInterface::ProcessInput() {
        std::string line;
        std::cout << PROMPT;
        
        if (!std::getline(std::cin, line)) {
            return false;
        }

        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        // Skip empty lines
        if (line.empty()) {
            return true;
        }

        // Check for quit
        if (line == "quit" || line == "exit") {
            return false;
        }

        // Execute command
        auto& console = DeveloperConsole::GetInstance();
        console.ExecuteCommand(line);

        return true;
    }

    void ConsoleInterface::Run() {
        auto& console = DeveloperConsole::GetInstance();
        
        // Register all FPS-specific commands and variables
        FPSConsoleCommands::RegisterAllCommands();

        PrintWelcomeMessage();

        // Main console loop
        while (ProcessInput()) {
            // Continue processing
        }

        std::cout << "[Console] Shutdown complete\n";
    }

} // namespace Archura
