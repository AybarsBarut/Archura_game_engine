#pragma once

#include <string>
#include <functional>
#include <unordered_map>
#include <vector>

namespace Archura {

    using CommandCallback = std::function<void(const std::vector<std::string>& args)>;

    class CommandRegistry {
    public:
        static CommandRegistry& Get();

        void RegisterCommand(const std::string& name, CommandCallback callback);
        void Dispatch(const std::string& commandLine);

    private:
        std::unordered_map<std::string, CommandCallback> m_Commands;
    };

} // namespace Archura
