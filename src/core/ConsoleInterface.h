#pragma once

#include <string>
#include <iostream>

namespace Archura {

    /**
     * @class ConsoleInterface
     * @brief Interactive console interface for the game
     * 
     * Provides a command-line interface where users can input and execute commands.
     * This runs in a separate thread or as a debug feature.
     */
    class ConsoleInterface {
    public:
        /**
         * Start the interactive console (blocking call)
         * Should be run in a separate thread
         */
        static void Run();

        /**
         * Single command input/output cycle
         * @return true if should continue, false if quit command received
         */
        static bool ProcessInput();

        /**
         * Print the console prompt and help message
         */
        static void PrintWelcomeMessage();

    private:
        static constexpr const char* PROMPT = ">>> ";
        static constexpr const char* VERSION = "1.0";
    };

} // namespace Archura
