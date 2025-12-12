#include "CommandRegistry.h"
#include <sstream>
#include <iostream>

namespace Archura {

    CommandRegistry& CommandRegistry::Get() {
        static CommandRegistry instance;
        return instance;
    }

    void CommandRegistry::RegisterCommand(const std::string& name, CommandCallback callback) {
        m_Commands[name] = callback;
    }

    void CommandRegistry::Dispatch(const std::string& commandLine) {
        if (commandLine.empty()) return;

        std::stringstream ss(commandLine);
        std::string commandName;
        ss >> commandName;

        // Remove slash if present (e.g. /dev -> dev)
        if (commandName.size() > 0 && commandName[0] == '/') {
            commandName = commandName.substr(1);
        }

        std::vector<std::string> args;
        std::string arg;
        while (ss >> arg) {
            args.push_back(arg);
        }

        auto it = m_Commands.find(commandName);
        if (it != m_Commands.end()) {
            it->second(args);
        } else {
            // Unknown command (silently ignore for CLI, Editor handles its own logging)
        }
    }

} // namespace Archura
