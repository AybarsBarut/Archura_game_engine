#pragma once

#include "../core/DeveloperConsole.h"
#include <memory>

namespace Archura {

    /**
     * @class FPSConsoleCommands
     * @brief FPS oyununa özel konsol komutları ve değişkenleri kaydeder
     */
    class FPSConsoleCommands {
    public:
        static void RegisterAllCommands();

    private:
        static void RegisterRenderingCommands();
        static void RegisterNetworkingCommands();
        static void RegisterGameplayCommands();
        static void RegisterAudioCommands();
        static void RegisterCheatCommands();
        static void RegisterSystemCommands();
        static void RegisterBotCommands();
    };

} // namespace Archura
