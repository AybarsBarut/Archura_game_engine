// Archura FPS Engine - Console System Integration Example
// Bu dosya konsol sisteminin nasıl entegre edileceğini gösterir

#include "core/DeveloperConsole.h"
#include "core/ConsoleInterface.h"
#include "game/FPSConsoleCommands.h"
#include <iostream>
#include <thread>

namespace Archura {

    /**
     * @class GameEngine
     * @brief Basit oyun motoru örneği - Konsol sistemi entegrasyonu
     */
    class GameEngine {
    public:
        GameEngine() : m_running(false) {}

        /**
         * Oyun motorunu başlat
         */
        void Initialize() {
            std::cout << "[Engine] Initializing Archura FPS Engine...\n";
            
            // Konsol sistemini başlat
            auto& console = DeveloperConsole::GetInstance();
            FPSConsoleCommands::RegisterAllCommands();
            
            std::cout << "[Engine] Console system initialized\n";
            std::cout << "[Engine] Type 'help' in console for more information\n";
        }

        /**
         * Ana oyun döngüsü
         */
        void Run() {
            m_running = true;
            Initialize();

            // Eğer console argümanı varsa, konsolu ayrı thread'de çalıştır
            std::thread consoleThread([]() {
                ConsoleInterface::Run();
            });

            // Burada asıl oyun döngüsü devam eder
            MainGameLoop();

            // Konsol thread'i bitene kadar bekle
            consoleThread.join();
        }

        /**
         * Ana oyun döngüsü örneği
         */
        void MainGameLoop() {
            std::cout << "\n[Engine] Game loop started. Type commands in console.\n";
            std::cout << "[Engine] Type 'quit' in console to exit.\n\n";

            while (m_running) {
                // Oyun tick'i simüle et
                Update();
                Render();
                std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
            }

            std::cout << "\n[Engine] Shutting down...\n";
        }

    private:
        void Update() {
            // Oyun mantığı güncellemesi
            auto& console = DeveloperConsole::GetInstance();
            
            // Örneğin: GOD modunu kontrol et
            auto godMode = console.GetVariable("sv_cheats");
            // ... oyun güncelleme kodu
        }

        void Render() {
            // Grafik rendering
            auto& console = DeveloperConsole::GetInstance();
            
            // Örneğin: FPS göstergesi
            auto fpsDisplay = console.GetVariable("r_fps_display");
            // ... rendering kodu
        }

        bool m_running;
    };

} // namespace Archura

/**
 * ENTEGRASYON ADIMLARI
 * 
 * 1. DeveloperConsole::GetInstance() ile singleton'a erişin
 * 2. FPSConsoleCommands::RegisterAllCommands() ile komutları kaydedin
 * 3. ConsoleInterface::Run() ile interaktif konsol başlatın
 * 4. DeveloperConsole'dan değişken değerlerini okuyun ve oyuna uygulayın
 * 
 * ÖRNEK OYUN KÖDÜ:
 * 
 * // Frame başında
 * auto& console = DeveloperConsole::GetInstance();
 * 
 * // Değişkenleri oku
 * float fov = console.GetVariable("r_fov")->GetAsFloat();
 * float sensitivity = console.GetVariable("m_sensitivity")->GetAsFloat();
 * bool godMode = console.GetVariable("sv_cheats")->GetAsBool();
 * 
 * // Oyunda kullan
 * camera.SetFOV(fov);
 * input.SetSensitivity(sensitivity);
 * player.SetGodMode(godMode);
 */

// EXAMPLE USAGE IN GAME ENGINE
int main() {
    try {
        Archura::GameEngine engine;
        engine.Run();
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

/**
 * KONSOL SISTEMI ARKİTEKTÜRÜ
 * 
 * ┌─────────────────────────────────────┐
 * │     ConsoleInterface (UI)           │
 * │  - Kullanıcı komut input/output     │
 * └──────────────┬──────────────────────┘
 *                │
 * ┌──────────────▼──────────────────────┐
 * │   DeveloperConsole (Singleton)      │
 * │  - Komut yönetimi                   │
 * │  - CVAR yönetimi                    │
 * │  - Komut parsing                    │
 * └──────────────┬──────────────────────┘
 *                │
 * ┌──────────────▼──────────────────────┐
 * │   FPSConsoleCommands (Registrar)    │
 * │  - Render komutları (r_)            │
 * │  - Network komutları (net_)         │
 * │  - Gameplay komutları (m_, cl_)     │
 * │  - Audio komutları (snd_)           │
 * │  - Cheat komutları (sv_) [ADMIN]    │
 * │  - System komutları (sys_)          │
 * │  - Bot komutları (bot_)             │
 * └─────────────────────────────────────┘
 * 
 * VERI AKIŞI:
 * 
 * Input: "r_fov 100"
 *   ↓
 * ConsoleInterface::ProcessInput()
 *   ↓
 * DeveloperConsole::ExecuteCommand("r_fov", ["100"])
 *   ↓
 * CVAR bulunur ve değeri güncellenir
 *   ↓
 * Game Engine: console.GetVariable("r_fov")->GetAsFloat()
 *   ↓
 * Camera.SetFOV(100.0f)
 */
