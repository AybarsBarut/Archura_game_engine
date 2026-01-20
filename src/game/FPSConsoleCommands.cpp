#include "FPSConsoleCommands.h"
#include "../core/DeveloperConsole.h"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <chrono>

namespace Archura {

    // Simulated game state for demo
    struct GameState {
        static int godMode;
        static int noclipMode;
        static int infiniteAmmo;
        static std::vector<std::string> activeBots;
        static bool profileRunning;
        static std::chrono::high_resolution_clock::time_point profileStartTime;
    };

    int GameState::godMode = 0;
    int GameState::noclipMode = 0;
    int GameState::infiniteAmmo = 0;
    std::vector<std::string> GameState::activeBots;
    bool GameState::profileRunning = false;
    std::chrono::high_resolution_clock::time_point GameState::profileStartTime;

    void FPSConsoleCommands::RegisterAllCommands() {
        RegisterRenderingCommands();
        RegisterNetworkingCommands();
        RegisterGameplayCommands();
        RegisterAudioCommands();
        RegisterCheatCommands();
        RegisterSystemCommands();
        RegisterBotCommands();
    }

    void FPSConsoleCommands::RegisterRenderingCommands() {
        auto& console = DeveloperConsole::GetInstance();

        // === Rendering Commands ===

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "r_stats",
            "Display rendering statistics",
            [](const std::vector<std::string>& args) {
                std::cout << "\n=== Rendering Statistics ===\n";
                std::cout << "Draw Calls: 1234\n";
                std::cout << "Polygons: 5,234,567\n";
                std::cout << "Textures: 342\n";
                std::cout << "VRAM Usage: 2048 MB\n";
                std::cout << "Shader Programs: 45\n";
                std::cout << "=============================\n\n";
            }
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "r_reload_shaders",
            "Reload all shader programs",
            [](const std::vector<std::string>& args) {
                std::cout << "[Rendering] Reloading shaders...\n";
                std::cout << "[Rendering] 45 shader programs reloaded successfully\n";
            }
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "r_texture_reload",
            "Reload all textures from disk",
            [](const std::vector<std::string>& args) {
                std::cout << "[Rendering] Reloading textures...\n";
                std::cout << "[Rendering] 342 textures reloaded (total: 2048 MB)\n";
            }
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "r_dump_statistics",
            "Dump detailed rendering statistics to file",
            [](const std::vector<std::string>& args) {
                std::string filename = args.empty() ? "render_stats.txt" : args[0];
                std::cout << "[Rendering] Statistics saved to " << filename << "\n";
            }
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "r_clear_cache",
            "Clear texture and shader cache",
            [](const std::vector<std::string>& args) {
                std::cout << "[Rendering] Clearing cache...\n";
                std::cout << "[Rendering] Cache cleared (freed 512 MB)\n";
            }
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "r_list_textures",
            "List all loaded textures",
            [](const std::vector<std::string>& args) {
                std::cout << "\n=== Loaded Textures (showing first 10) ===\n";
                std::cout << "1. diffuse_floor.dds (512x512) 1 MB\n";
                std::cout << "2. normal_wall.dds (1024x1024) 4 MB\n";
                std::cout << "3. metallic_surface.dds (2048x2048) 16 MB\n";
                std::cout << "4. wood_texture.dds (512x512) 1 MB\n";
                std::cout << "5. concrete_floor.dds (1024x1024) 4 MB\n";
                std::cout << "... and 337 more textures\n";
                std::cout << "Total: 342 textures, 2048 MB\n";
                std::cout << "==========================================\n\n";
            }
        ));

        // Rendering Variables
        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "r_model_quality",
            "3",
            ConsoleVariable::Type::Int,
            "Model detail level (0=Low, 4=Ultra)"
        ));

        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "r_shadow_distance",
            "1000",
            ConsoleVariable::Type::Float,
            "Shadow render distance in meters"
        ));

        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "r_ao_enabled",
            "1",
            ConsoleVariable::Type::Bool,
            "Ambient Occlusion enabled"
        ));

        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "r_bloom_intensity",
            "1.0",
            ConsoleVariable::Type::Float,
            "Bloom effect intensity"
        ));

        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "r_gamma",
            "1.0",
            ConsoleVariable::Type::Float,
            "Screen gamma correction (0.5-2.5)"
        ));

        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "r_motion_blur",
            "0",
            ConsoleVariable::Type::Bool,
            "Motion blur effect enabled"
        ));

        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "r_lod_bias",
            "0.0",
            ConsoleVariable::Type::Float,
            "LOD distance bias"
        ));
    }

    void FPSConsoleCommands::RegisterNetworkingCommands() {
        auto& console = DeveloperConsole::GetInstance();

        // === Networking Commands ===

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "net_ping",
            "Display current ping to server",
            [](const std::vector<std::string>& args) {
                int ping = 45 + (rand() % 30); // Simulated ping
                std::cout << "[Network] Ping: " << ping << " ms\n";
            }
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "net_stats",
            "Display network statistics",
            [](const std::vector<std::string>& args) {
                std::cout << "\n=== Network Statistics ===\n";
                std::cout << "Ping: 45 ms\n";
                std::cout << "Packet Loss: 0.2%\n";
                std::cout << "Download: 2.4 Mbps\n";
                std::cout << "Upload: 0.8 Mbps\n";
                std::cout << "Packets Sent: 234,567\n";
                std::cout << "Packets Received: 234,521\n";
                std::cout << "Bytes Sent: 12.5 MB\n";
                std::cout << "Bytes Received: 45.2 MB\n";
                std::cout << "===========================\n\n";
            }
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "connect",
            "Connect to server [IP:Port]",
            [](const std::vector<std::string>& args) {
                if (args.empty()) {
                    std::cout << "[Network] Usage: connect [IP:Port]\n";
                    return;
                }
                std::cout << "[Network] Connecting to " << args[0] << "...\n";
                std::cout << "[Network] Connected successfully\n";
            }
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "disconnect",
            "Disconnect from server",
            [](const std::vector<std::string>& args) {
                std::cout << "[Network] Disconnecting...\n";
                std::cout << "[Network] Disconnected\n";
            }
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "status",
            "Show connection status",
            [](const std::vector<std::string>& args) {
                std::cout << "\n=== Connection Status ===\n";
                std::cout << "Status: Connected\n";
                std::cout << "Server: 192.168.1.100:27015\n";
                std::cout << "Players: 32/32\n";
                std::cout << "Map: de_mirage\n";
                std::cout << "Time: 15:34\n";
                std::cout << "==========================\n\n";
            }
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "net_graph",
            "Display real-time network graph [TOGGLE]",
            [](const std::vector<std::string>& args) {
                std::cout << "[Network] Graph overlay toggled\n";
            }
        ));

        // Network Variables
        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "net_lerp",
            "0.015",
            ConsoleVariable::Type::Float,
            "Interpolation amount in seconds"
        ));

        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "net_lag_compensate",
            "1",
            ConsoleVariable::Type::Bool,
            "Server-side lag compensation"
        ));

        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "cl_predict_correct",
            "1",
            ConsoleVariable::Type::Bool,
            "Correct prediction errors"
        ));

        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "net_stats_display",
            "0",
            ConsoleVariable::Type::Bool,
            "Display network stats overlay"
        ));
    }

    void FPSConsoleCommands::RegisterGameplayCommands() {
        auto& console = DeveloperConsole::GetInstance();

        // === Gameplay Commands ===

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "bind",
            "Bind key to command: bind [key] [command]",
            [](const std::vector<std::string>& args) {
                if (args.size() < 2) {
                    std::cout << "[Input] Usage: bind [key] [command]\n";
                    return;
                }
                std::cout << "[Input] Bound '" << args[0] << "' to '" << args[1] << "'\n";
            }
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "unbind",
            "Unbind key from command: unbind [key]",
            [](const std::vector<std::string>& args) {
                if (args.empty()) {
                    std::cout << "[Input] Usage: unbind [key]\n";
                    return;
                }
                std::cout << "[Input] Unbound '" << args[0] << "'\n";
            }
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "bindlist",
            "List all key bindings",
            [](const std::vector<std::string>& args) {
                std::cout << "\n=== Key Bindings ===\n";
                std::cout << "W: +forward\n";
                std::cout << "A: +moveleft\n";
                std::cout << "S: +back\n";
                std::cout << "D: +moveright\n";
                std::cout << "SPACE: +jump\n";
                std::cout << "CTRL: +duck\n";
                std::cout << "MOUSE1: +attack\n";
                std::cout << "MOUSE2: +attack2\n";
                std::cout << "R: +reload\n";
                std::cout << "E: +use\n";
                std::cout << "====================\n\n";
            }
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "unbindall",
            "Remove all key bindings",
            [](const std::vector<std::string>& args) {
                std::cout << "[Input] All bindings cleared\n";
            }
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "map",
            "Load a map: map [map_name]",
            [](const std::vector<std::string>& args) {
                if (args.empty()) {
                    std::cout << "[Game] Usage: map [map_name]\n";
                    return;
                }
                std::cout << "[Game] Loading map: " << args[0] << "...\n";
                std::cout << "[Game] Map loaded\n";
            }
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "maplist",
            "List all available maps",
            [](const std::vector<std::string>& args) {
                std::cout << "\n=== Available Maps ===\n";
                std::cout << "de_mirage\n";
                std::cout << "de_inferno\n";
                std::cout << "de_cache\n";
                std::cout << "de_dust2\n";
                std::cout << "de_train\n";
                std::cout << "de_nuke\n";
                std::cout << "de_ancient\n";
                std::cout << "=======================\n\n";
            }
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "game_mode",
            "Set game mode: game_mode [mode]",
            [](const std::vector<std::string>& args) {
                if (args.empty()) {
                    std::cout << "[Game] Usage: game_mode [deathmatch|competitive|casual]\n";
                    return;
                }
                std::cout << "[Game] Game mode set to: " << args[0] << "\n";
            }
        ));

        // Gameplay Variables
        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "cl_jump_height",
            "1.0",
            ConsoleVariable::Type::Float,
            "Jump height multiplier"
        ));

        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "cl_move_speed",
            "1.0",
            ConsoleVariable::Type::Float,
            "Movement speed multiplier"
        ));

        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "cl_crouch_speed",
            "0.5",
            ConsoleVariable::Type::Float,
            "Crouch speed multiplier"
        ));

        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "m_sensitivity_ads",
            "0.75",
            ConsoleVariable::Type::Float,
            "Mouse sensitivity when aiming"
        ));

        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "m_invert_y",
            "0",
            ConsoleVariable::Type::Bool,
            "Invert mouse Y axis"
        ));

        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "cl_crosshair_color",
            "0 255 0",
            ConsoleVariable::Type::String,
            "Crosshair color (R G B)"
        ));

        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "cl_crosshair_alpha",
            "1.0",
            ConsoleVariable::Type::Float,
            "Crosshair transparency (0.0-1.0)"
        ));

        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "hud_scale",
            "1.0",
            ConsoleVariable::Type::Float,
            "HUD scale"
        ));

        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "hud_opacity",
            "1.0",
            ConsoleVariable::Type::Float,
            "HUD transparency (0.0-1.0)"
        ));

        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "hud_minimap",
            "1",
            ConsoleVariable::Type::Bool,
            "Display mini map"
        ));

        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "hud_scoreboard",
            "1",
            ConsoleVariable::Type::Bool,
            "Display scoreboard"
        ));
    }

    void FPSConsoleCommands::RegisterAudioCommands() {
        auto& console = DeveloperConsole::GetInstance();

        // === Audio Commands ===

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "snd_test",
            "Test audio system: snd_test [sound_name]",
            [](const std::vector<std::string>& args) {
                std::string sound = args.empty() ? "default_beep" : args[0];
                std::cout << "[Audio] Playing test sound: " << sound << "\n";
            }
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "snd_stats",
            "Display audio statistics",
            [](const std::vector<std::string>& args) {
                std::cout << "\n=== Audio Statistics ===\n";
                std::cout << "Active Channels: 45/128\n";
                std::cout << "Total Sounds Loaded: 234\n";
                std::cout << "Audio Memory: 512 MB\n";
                std::cout << "Sample Rate: 48000 Hz\n";
                std::cout << "Format: 5.1 Surround\n";
                std::cout << "=========================\n\n";
            }
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "snd_list_sounds",
            "List all loaded sounds",
            [](const std::vector<std::string>& args) {
                std::cout << "\n=== Loaded Sounds (showing first 10) ===\n";
                std::cout << "1. weapons/rifle/fire.wav (45 KB, 44kHz)\n";
                std::cout << "2. weapons/pistol/fire.wav (32 KB, 44kHz)\n";
                std::cout << "3. footsteps/concrete.wav (128 KB, 48kHz)\n";
                std::cout << "4. footsteps/metal.wav (140 KB, 48kHz)\n";
                std::cout << "5. ui/button_click.wav (8 KB, 44kHz)\n";
                std::cout << "... and 229 more sounds\n";
                std::cout << "Total: 234 sounds, 512 MB\n";
                std::cout << "=========================================\n\n";
            }
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "snd_reload",
            "Reload audio system",
            [](const std::vector<std::string>& args) {
                std::cout << "[Audio] Reloading audio system...\n";
                std::cout << "[Audio] Audio system reloaded\n";
            }
        ));

        // Audio Variables - additional
        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "snd_voice_volume",
            "0.85",
            ConsoleVariable::Type::Float,
            "Voice chat volume"
        ));

        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "snd_doppler_effect",
            "1",
            ConsoleVariable::Type::Bool,
            "Enable Doppler effect"
        ));

        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "snd_reverb_enabled",
            "1",
            ConsoleVariable::Type::Bool,
            "Enable reverb/echo effects"
        ));

        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "snd_channels",
            "128",
            ConsoleVariable::Type::Int,
            "Number of audio channels"
        ));

        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "snd_quality",
            "2",
            ConsoleVariable::Type::Int,
            "Audio quality (0=Low, 1=Medium, 2=High)"
        ));
    }

    void FPSConsoleCommands::RegisterCheatCommands() {
        auto& console = DeveloperConsole::GetInstance();

        // === Cheat Commands (require sv_cheats 1) ===

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "god",
            "God mode - take no damage [CHEATS]",
            [](const std::vector<std::string>& args) {
                GameState::godMode = GameState::godMode ? 0 : 1;
                std::cout << "[Cheats] God mode " << (GameState::godMode ? "ENABLED" : "DISABLED") << "\n";
            },
            true // requires cheats
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "buddha",
            "Buddha mode - take damage but don't die [CHEATS]",
            [](const std::vector<std::string>& args) {
                std::cout << "[Cheats] Buddha mode toggled\n";
            },
            true
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "noclip",
            "No clip - walk through walls [CHEATS]",
            [](const std::vector<std::string>& args) {
                GameState::noclipMode = GameState::noclipMode ? 0 : 1;
                std::cout << "[Cheats] Noclip mode " << (GameState::noclipMode ? "ENABLED" : "DISABLED") << "\n";
            },
            true
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "notarget",
            "Enemies ignore you [CHEATS]",
            [](const std::vector<std::string>& args) {
                std::cout << "[Cheats] Notarget toggled\n";
            },
            true
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "give",
            "Give item: give [item_name] [amount] [CHEATS]",
            [](const std::vector<std::string>& args) {
                if (args.empty()) {
                    std::cout << "[Cheats] Usage: give [item_name] [amount]\n";
                    std::cout << "[Cheats] Examples: give rifle_m4, give grenade 5, give armor\n";
                    return;
                }
                int amount = args.size() > 1 ? std::atoi(args[1].c_str()) : 1;
                std::cout << "[Cheats] Given " << amount << "x " << args[0] << "\n";
            },
            true
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "impulse",
            "Execute impulse code: impulse [code] [CHEATS]",
            [](const std::vector<std::string>& args) {
                if (args.empty()) {
                    std::cout << "[Cheats] Usage: impulse [code]\n";
                    std::cout << "[Cheats] impulse 101 = Give all weapons\n";
                    return;
                }
                if (args[0] == "101") {
                    std::cout << "[Cheats] Given all weapons and ammo\n";
                }
                else {
                    std::cout << "[Cheats] Impulse " << args[0] << " executed\n";
                }
            },
            true
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "sv_hitbox_debug",
            "Display hitboxes for debugging [CHEATS]",
            [](const std::vector<std::string>& args) {
                std::cout << "[Cheats] Hitbox visualization toggled\n";
            },
            true
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "sv_wireframe",
            "Wireframe rendering mode [CHEATS]",
            [](const std::vector<std::string>& args) {
                std::cout << "[Cheats] Wireframe mode toggled\n";
            },
            true
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "sv_skeleton_debug",
            "Display character skeletons [CHEATS]",
            [](const std::vector<std::string>& args) {
                std::cout << "[Cheats] Skeleton visualization toggled\n";
            },
            true
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "sv_bullet_trails",
            "Show bullet trajectories [CHEATS]",
            [](const std::vector<std::string>& args) {
                std::cout << "[Cheats] Bullet trail visualization toggled\n";
            },
            true
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "sv_trace_rays",
            "Visualize raycasts [CHEATS]",
            [](const std::vector<std::string>& args) {
                std::cout << "[Cheats] Raycast visualization toggled\n";
            },
            true
        ));

        // Cheat Variables
        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "sv_infinite_ammo",
            "0",
            ConsoleVariable::Type::Bool,
            "Infinite ammunition [CHEATS]",
            true
        ));

        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "sv_blood_enabled",
            "1",
            ConsoleVariable::Type::Bool,
            "Blood effects [CHEATS]",
            true
        ));

        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "sv_impact_particles",
            "1",
            ConsoleVariable::Type::Bool,
            "Bullet impact particles [CHEATS]",
            true
        ));
    }

    void FPSConsoleCommands::RegisterSystemCommands() {
        auto& console = DeveloperConsole::GetInstance();

        // === System Commands ===

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "sys_info",
            "Display system information",
            [](const std::vector<std::string>& args) {
                std::cout << "\n=== System Information ===\n";
                std::cout << "OS: Windows 11 Pro\n";
                std::cout << "CPU: Intel Core i9-13900K\n";
                std::cout << "RAM: 32 GB DDR5\n";
                std::cout << "GPU: NVIDIA RTX 4090\n";
                std::cout << "VRAM: 24 GB GDDR6X\n";
                std::cout << "Engine: Archura FPS Engine v1.0\n";
                std::cout << "===========================\n\n";
            }
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "sys_benchmark",
            "Run performance benchmark",
            [](const std::vector<std::string>& args) {
                std::cout << "[System] Running benchmark...\n";
                std::cout << "[System] CPU: 1250 points\n";
                std::cout << "[System] GPU: 2850 points\n";
                std::cout << "[System] Memory: 1540 points\n";
                std::cout << "[System] Total Score: 5640 points\n";
            }
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "profile_start",
            "Start performance profiling: profile_start [name]",
            [](const std::vector<std::string>& args) {
                std::string name = args.empty() ? "default" : args[0];
                GameState::profileRunning = true;
                GameState::profileStartTime = std::chrono::high_resolution_clock::now();
                std::cout << "[Profiling] Profiling started: " << name << "\n";
            }
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "profile_stop",
            "Stop performance profiling and display results",
            [](const std::vector<std::string>& args) {
                if (!GameState::profileRunning) {
                    std::cout << "[Profiling] No active profile\n";
                    return;
                }
                GameState::profileRunning = false;
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                    end - GameState::profileStartTime
                );
                std::cout << "[Profiling] Profile duration: " << duration.count() << " ms\n";
                std::cout << "[Profiling] Rendering: " << (duration.count() * 0.6) << " ms\n";
                std::cout << "[Profiling] Simulation: " << (duration.count() * 0.3) << " ms\n";
                std::cout << "[Profiling] Other: " << (duration.count() * 0.1) << " ms\n";
            }
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "profile_results",
            "Display last profile results",
            [](const std::vector<std::string>& args) {
                std::cout << "\n=== Profile Results ===\n";
                std::cout << "Total Time: 150.23 ms\n";
                std::cout << "- Rendering: 90.14 ms (60%)\n";
                std::cout << "- Simulation: 45.07 ms (30%)\n";
                std::cout << "- Other: 15.02 ms (10%)\n";
                std::cout << "========================\n\n";
            }
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "log_enable",
            "Enable/disable file logging: log_enable [0/1]",
            [](const std::vector<std::string>& args) {
                bool enable = args.empty() || args[0] != "0";
                std::cout << "[System] Logging " << (enable ? "ENABLED" : "DISABLED") << "\n";
            }
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "log_clear",
            "Clear log file",
            [](const std::vector<std::string>& args) {
                std::cout << "[System] Log file cleared\n";
            }
        ));

        // System Variables
        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "sys_frame_analysis",
            "0",
            ConsoleVariable::Type::Bool,
            "Display detailed frame analysis"
        ));

        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "log_level",
            "2",
            ConsoleVariable::Type::Int,
            "Log verbosity level (0-4)"
        ));

        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "log_file",
            "game.log",
            ConsoleVariable::Type::String,
            "Log file name"
        ));

        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "log_show_timestamp",
            "1",
            ConsoleVariable::Type::Bool,
            "Include timestamps in logs"
        ));
    }

    void FPSConsoleCommands::RegisterBotCommands() {
        auto& console = DeveloperConsole::GetInstance();

        // === Bot Commands ===

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "bot_add",
            "Add a bot: bot_add [team] [difficulty] [name] [CHEATS]",
            [](const std::vector<std::string>& args) {
                std::string team = args.size() > 0 ? args[0] : "ct";
                std::string difficulty = args.size() > 1 ? args[1] : "easy";
                std::string name = args.size() > 2 ? args[2] : "Bot_" + std::to_string(GameState::activeBots.size() + 1);
                
                GameState::activeBots.push_back(name);
                std::cout << "[Bot] Added bot '" << name << "' (Team: " << team << ", Difficulty: " << difficulty << ")\n";
            },
            true
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "bot_remove",
            "Remove a bot: bot_remove [bot_name] [CHEATS]",
            [](const std::vector<std::string>& args) {
                if (args.empty()) {
                    std::cout << "[Bot] Usage: bot_remove [bot_name]\n";
                    return;
                }
                auto it = std::find(GameState::activeBots.begin(), GameState::activeBots.end(), args[0]);
                if (it != GameState::activeBots.end()) {
                    GameState::activeBots.erase(it);
                    std::cout << "[Bot] Removed bot '" << args[0] << "'\n";
                }
                else {
                    std::cout << "[Bot] Bot '" << args[0] << "' not found\n";
                }
            },
            true
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "bot_removeall",
            "Remove all bots [CHEATS]",
            [](const std::vector<std::string>& args) {
                int count = GameState::activeBots.size();
                GameState::activeBots.clear();
                std::cout << "[Bot] Removed " << count << " bots\n";
            },
            true
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "bot_list",
            "List all active bots",
            [](const std::vector<std::string>& args) {
                std::cout << "\n=== Active Bots ===\n";
                if (GameState::activeBots.empty()) {
                    std::cout << "No active bots\n";
                }
                else {
                    for (size_t i = 0; i < GameState::activeBots.size(); ++i) {
                        std::cout << (i + 1) << ". " << GameState::activeBots[i] << "\n";
                    }
                }
                std::cout << "===================\n\n";
            }
        ));

        console.RegisterCommand(std::make_shared<ConsoleCommand>(
            "bot_kick",
            "Kick all bots [CHEATS]",
            [](const std::vector<std::string>& args) {
                int count = GameState::activeBots.size();
                GameState::activeBots.clear();
                std::cout << "[Bot] Kicked " << count << " bots\n";
            },
            true
        ));

        // Bot Variables
        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "bot_difficulty",
            "1",
            ConsoleVariable::Type::Int,
            "Default bot difficulty (0=Very Easy, 3=Hard) [CHEATS]",
            true
        ));

        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "bot_aggressiveness",
            "0.5",
            ConsoleVariable::Type::Float,
            "Bot aggressiveness (0.0-1.0) [CHEATS]",
            true
        ));

        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "bot_accuracy",
            "0.7",
            ConsoleVariable::Type::Float,
            "Bot aim accuracy (0.0-1.0) [CHEATS]",
            true
        ));

        console.RegisterVariable(std::make_shared<ConsoleVariable>(
            "bot_reaction_time",
            "100",
            ConsoleVariable::Type::Int,
            "Bot reaction time in milliseconds [CHEATS]",
            true
        ));
    }

} // namespace Archura
