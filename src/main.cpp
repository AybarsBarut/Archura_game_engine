#include "network/NetworkManager.h"
#include "core/Engine.h"
#include "core/Window.h"
#include "input/Input.h"
#include "rendering/Renderer.h"
#include <GLFW/glfw3.h>
#include "core/ResourceManager.h"
#include "ecs/Entity.h"
#include "ecs/Component.h"
#include "rendering/Mesh.h"
#include "rendering/Camera.h"
#include "rendering/HUDRenderer.h"
#include "rendering/Skybox.h"
#include "game/FPSController.h"
#include "game/RenderSystem.h"
#include "game/Weapon.h"
#include "game/Projectile.h"
#include "editor/Editor.h"
#include "game/DevConsole.h"
#include "core/AudioSystem.h"
#include <iostream>
#include <memory>
#include <ctime>
#include <cstdlib>
#include <imgui.h>
#include <string>
#include <fstream>

using namespace Archura;

// Harici GPU kullanimi icin ipucu (NVIDIA ve AMD)
extern "C" {
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int main() {
    // Motor yapilandirmasi
    Engine::EngineConfig config;
    config.windowTitle = "Archura FPS Engine - Demo";
    config.windowWidth = 1920;
    config.windowHeight = 1080;
    config.vsync = false; // VSync kapali = sinirsiz FPS
    config.fullscreen = false;
    config.editorMode = false; // Simdilik editor arayuzu kapali

    // Motoru baslat
    auto& engine = Engine::Get();
    if (!engine.Init(config)) {
        std::cerr << "Failed to initialize engine!" << std::endl;
        return -1;
    }

    std::cout << "\n=== Archura Engine Initialization ===" << std::endl;
    
    // Surum bilgisini yazdir
    std::ifstream versionFile("version.txt");
    if (versionFile.is_open()) {
        std::string version;
        std::getline(versionFile, version);
        std::cout << "Engine Version: " << version << std::endl;
        versionFile.close();
    } else {
        std::cout << "Engine Version: Unknown (version.txt not found)" << std::endl;
    }

    std::cout << "=== Creating Demo Scene ===" << std::endl;

    // Sahne olustur
    Scene scene("Demo Scene");

    // Kamera ve FPS kontrolcusu
    Camera camera(glm::vec3(0.0f, 2.0f, 10.0f));
    camera.SetMovementSpeed(5.0f);
    FPSController fpsController(&camera);

    // Goruntu sistemi
    RenderSystem renderSystem(&camera);
    renderSystem.Init(&scene);

    // Skybox
    Skybox skybox;
    skybox.Init();

    // HUD Isleyici
    HUDRenderer hudRenderer;
    if (!hudRenderer.Init()) {
        std::cerr << "Failed to initialize HUD renderer!" << std::endl;
        return -1;
    }
    hudRenderer.SetScreenSize(1920.0f, 1080.0f);

    // Editor (ImGui)
    Editor editor;
    if (!editor.Init(engine.GetWindow())) {
        std::cerr << "Failed to initialize Editor!" << std::endl;
        return -1;
    }
    editor.SetEnabled(true); // Editor aktif

    // Oyuncu varligi (silah tasiyicisi)
    // Oyuncu varligi (silah tasiyicisi)
    Entity* player = scene.CreateEntity("Player");
    auto* weapon = player->AddComponent<Weapon>();
    weapon->InitInventory();
    
    // Oyuncu cani
    auto* playerHealthComp = player->AddComponent<Health>();
    playerHealthComp->current = 100.0f;
    playerHealthComp->max = 100.0f;
    
    // Oyuncu carpisma kutusu (dusmanlarin vurabilmesi icin)
    auto* playerCollider = player->AddComponent<BoxCollider>();
    playerCollider->size = glm::vec3(0.8f, 1.8f, 0.8f); // Human size

    // Silah sistemi
    WeaponSystem weaponSystem;

    // Mermi sistemi
    ProjectileSystem projectileSystem;

    // Dev Console Baslat
    DevConsole::Get().Init();

    // Ses Sistemi Baslat
    AudioSystem::Get().Init();
    // Ornek: Arkaplan muzigi (dosya varsa calar)
    AudioSystem::Get().PlayMusic("bgm.mp3");

    // Ag Geri Cagrisi - Ates Etme


    // ... (Existing Network Update Callback) ...

    // ... (Inside Loop) ...



    // ==================== Demo Varliklar ====================
    
    // ==================== Harita Olusturma ====================
    
    // Zemin (Platform)
    Entity* ground = scene.CreateEntity("Ground");
    auto* groundMesh = ground->AddComponent<MeshRenderer>();
    groundMesh->mesh = Mesh::CreatePlane(100.0f, 100.0f);
    groundMesh->color = glm::vec3(0.2f, 0.25f, 0.2f); // Koyu gri-yesil zemin
    auto* groundTransform = ground->GetComponent<Transform>();
    groundTransform->position = glm::vec3(0.0f, 0.0f, 0.0f);

    // Duvar Materyali (Gri Beton)
    glm::vec3 wallColor = glm::vec3(0.5f, 0.5f, 0.55f);
    glm::vec3 obstacleColor = glm::vec3(0.6f, 0.4f, 0.3f); // Kahverengi kutular

    // Harita Sinirlari (Dis Duvarlar)
    float mapSize = 50.0f; // 100x100 zemin icin -50 ile +50 arasi
    float wallHeight = 5.0f;
    float wallThickness = 2.0f;

    struct WallDef {
        glm::vec3 pos;
        glm::vec3 scale;
        glm::vec3 color;
    };

    std::vector<WallDef> walls;

    // 4 Kenar Duvar
    walls.push_back({ glm::vec3(0, wallHeight/2, -mapSize), glm::vec3(mapSize*2, wallHeight, wallThickness), wallColor }); // Kuzey
    walls.push_back({ glm::vec3(0, wallHeight/2, mapSize), glm::vec3(mapSize*2, wallHeight, wallThickness), wallColor }); // Guney
    walls.push_back({ glm::vec3(-mapSize, wallHeight/2, 0), glm::vec3(wallThickness, wallHeight, mapSize*2), wallColor }); // Bati
    walls.push_back({ glm::vec3(mapSize, wallHeight/2, 0), glm::vec3(wallThickness, wallHeight, mapSize*2), wallColor }); // Dogu

    // Ic Duvarlar ve Engeller (Labirentimsi Yapi)
    // Sol taraf siperleri
    walls.push_back({ glm::vec3(-15, 2, -15), glm::vec3(10, 4, 2), wallColor });
    walls.push_back({ glm::vec3(-25, 2, -5), glm::vec3(2, 4, 15), wallColor });

    // Sag taraf siperleri
    walls.push_back({ glm::vec3(15, 2, 15), glm::vec3(10, 4, 2), wallColor });
    walls.push_back({ glm::vec3(25, 2, 5), glm::vec3(2, 4, 15), wallColor });

    // Orta Alan (Arena)
    walls.push_back({ glm::vec3(0, 1.5f, 0), glm::vec3(5, 3, 5), obstacleColor }); // Merkez kutu
    walls.push_back({ glm::vec3(-8, 1, 8), glm::vec3(3, 2, 3), obstacleColor });
    walls.push_back({ glm::vec3(8, 1, -8), glm::vec3(3, 2, 3), obstacleColor });

    // Rastgele dagilmis kucuk siperler
    walls.push_back({ glm::vec3(0, 1.5f, 25), glm::vec3(8, 3, 1), wallColor });
    walls.push_back({ glm::vec3(0, 1.5f, -25), glm::vec3(8, 3, 1), wallColor });
    walls.push_back({ glm::vec3(25, 1.5f, 0), glm::vec3(1, 3, 8), wallColor });
    walls.push_back({ glm::vec3(-25, 1.5f, 0), glm::vec3(1, 3, 8), wallColor });

    // Duvarlari Olustur
    int wallCount = 0;
    for (const auto& def : walls) {
        Entity* wall = scene.CreateEntity("Wall_" + std::to_string(wallCount++));
        
        // Mesh
        auto* mesh = wall->AddComponent<MeshRenderer>();
        mesh->mesh = Mesh::CreateCube(1.0f); // Temel kup, olcek ile buyutecegiz
        mesh->color = def.color;

        // Transform
        auto* transform = wall->GetComponent<Transform>();
        transform->position = def.pos;
        transform->scale = def.scale;

        // Collider
        auto* collider = wall->AddComponent<BoxCollider>();
        collider->size = glm::vec3(1.0f); // Model boyutu 1x1x1 oldugu icin carpisma kutusu da 1x1x1 (olcek ile carpilacak)
    }

    std::cout << "Scene created with " << scene.GetEntities().size() << " entities." << std::endl;
    std::cout << "\n=== Controls ===" << std::endl;
    std::cout << "  WASD: Move" << std::endl;
    std::cout << "  Q/E: Up/Down" << std::endl;
    std::cout << "  Right Mouse + Move: Look Around" << std::endl;
    std::cout << "  Left Mouse: Shoot" << std::endl;
    std::cout << "  R: Reload" << std::endl;
    std::cout << "  1-4: Switch Weapon Type" << std::endl;
    std::cout << "  Shift: Sprint" << std::endl;
    std::cout << "  Mouse Scroll: Zoom (FOV)" << std::endl;
    std::cout << "  TAB: Toggle Editor UI" << std::endl;
    std::cout << "  ESC: Exit" << std::endl;
    std::cout << "==================\n" << std::endl;

    // Ag Yoneticisi Baslatma
    auto& network = NetworkManager::Get();
    network.Init();

    // Uzak Oyuncular Haritasi
    std::unordered_map<uint32_t, Entity*> remotePlayers;

    // Ag Geri Cagrisi
    network.SetOnPlayerUpdate([&](const PlayerUpdatePacket& packet) {
        // Kendimizden gelen paketleri yoksay (Sunucu isek)
        // Basitlik icin ID kontrolu yapmiyoruz, her gelen paketi isliyoruz
        // Gercek uygulamada istemci ID kontrolu gerekir

        auto it = remotePlayers.find(packet.id);
        if (it == remotePlayers.end()) {
            // Yeni oyuncu olustur
            std::cout << "New remote player: " << packet.id << std::endl;
            Entity* newPlayer = scene.CreateEntity("RemotePlayer_" + std::to_string(packet.id));
            
            auto* mesh = newPlayer->AddComponent<MeshRenderer>();
            
            // Karakter modelini yuklemeyi dene
            Mesh* charMesh = Mesh::LoadFromOBJ("character.obj");
            auto* transform = newPlayer->GetComponent<Transform>();

            if (charMesh) {
                mesh->mesh = charMesh;
                mesh->color = glm::vec3(1.0f); // Modelin kendi renklerini/dokusunu kullan (eger varsa)
                transform->scale = glm::vec3(1.0f, 2.0f, 1.0f); // OBJ icin olcek (tahmini)
            } else {
                // Yedek: Kapsul (Insan boyutlarinda: r=0.4, h=1.8)
                mesh->mesh = Mesh::CreateCapsule(0.4f, 1.8f); 
                mesh->color = glm::vec3(0.0f, 0.5f, 1.0f); // Mavi renk
                transform->scale = glm::vec3(1.0f); // Kapsul zaten dogru boyutta uretiliyor
            }
            
            remotePlayers[packet.id] = newPlayer;
            it = remotePlayers.find(packet.id);
        }

        // Pozisyonu guncelle
        if (it != remotePlayers.end()) {
            auto* transform = it->second->GetComponent<Transform>();
            transform->position = glm::vec3(packet.x, packet.y, packet.z);
            transform->rotation = glm::vec3(packet.pitch, packet.yaw, 0.0f);
        }
    });

    // Ana oyun dongusu
    auto* window = engine.GetWindow();
    auto* input = engine.GetInput();
    auto* renderer = engine.GetRenderer();

    // Duraklatma Durumu
    bool isPaused = false;
    bool isKeyBindingMode = false;
    int* keyBindingTarget = nullptr;
    const char* keyBindingName = "";
    float escCooldown = 0.0f;
    float tabCooldown = 0.0f;

    // Ag Zamanlayicisi
    float networkTimer = 0.0f;
    const float networkTickRate = 0.05f; // 20 updates per second

    // Bu istemci icin rastgele ID
    srand((unsigned int)time(0));
    uint32_t myClientID = rand(); 
    
    // Network Callback - Shoot
    network.SetOnPlayerShoot([&](const PlayerShootPacket& packet) {
        if (packet.id == myClientID) return;

        Entity* owner = nullptr;
        auto it = remotePlayers.find(packet.id);
        if (it != remotePlayers.end()) {
            owner = it->second;
        }

        projectileSystem.SpawnProjectile(
            &scene,
            glm::vec3(packet.originX, packet.originY, packet.originZ),
            glm::vec3(packet.dirX, packet.dirY, packet.dirZ),
            50.0f, 
            10.0f, 
            owner
        );
    }); 

    while (!window->ShouldClose()) {
        float deltaTime = window->GetDeltaTime();
        
        // Bekleme suresi guncellemesi
        if (escCooldown > 0.0f) escCooldown -= deltaTime;
        if (tabCooldown > 0.0f) tabCooldown -= deltaTime;
        
        // Giris guncellemesi
        input->Update();

        // F1 tusu ile Gelistirici Konsolu
        if (input->IsKeyPressed(GLFW_KEY_F1)) {
            DevConsole::Get().Toggle();
            // Konsol acildiginda imleci serbest birak
            if (DevConsole::Get().IsOpen()) {
                input->SetCursorMode(GLFW_CURSOR_NORMAL);
            } else if (!isPaused && !editor.IsEnabled()) {
                // Konsol kapandiysa ve oyun duraklatilmadiysa imleci kilitle
                input->SetCursorMode(GLFW_CURSOR_DISABLED);
            }
        }

        // ESC tusu ile Duraklatma Menusu
        if (input->IsKeyPressed(GLFW_KEY_ESCAPE) && escCooldown <= 0.0f) {
            escCooldown = 0.2f; // 200ms bekleme suresi
            
            // Eger konsol aciksa, once onu kapat
            if (DevConsole::Get().IsOpen()) {
                DevConsole::Get().Toggle();
                if (!isPaused && !editor.IsEnabled()) {
                    input->SetCursorMode(GLFW_CURSOR_DISABLED);
                }
            } else {
                // Konsol kapaliysa normal duraklatma menusu
                if (isKeyBindingMode) {
                    // Tus atama iptal
                    isKeyBindingMode = false;
                    keyBindingTarget = nullptr;
                } else {
                    isPaused = !isPaused;
                    if (isPaused) {
                        input->SetCursorMode(GLFW_CURSOR_NORMAL);
                    } else {
                        input->SetCursorMode(GLFW_CURSOR_DISABLED);
                    }
                }
            }
        }

        // Tus Atama Modu
        if (isKeyBindingMode && keyBindingTarget) {
            // Herhangi bir tusa basildi mi?
            for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key) {
                if (input->IsKeyPressed(key) && key != GLFW_KEY_ESCAPE) {
                    *keyBindingTarget = key;
                    isKeyBindingMode = false;
                    keyBindingTarget = nullptr;
                    break;
                }
            }
        }

        // Oyun mantigi (Duraklatilmadiysa)
        if (!isPaused) {
            // Ag Guncellemeleri
            if (network.IsServer()) {
                network.UpdateServer();
            } else {
                network.UpdateClient();
            }

            // Oyuncu Guncellemesi Gonder
            if (network.IsConnected()) {
                networkTimer += deltaTime;
                if (networkTimer >= networkTickRate) {
                    PlayerUpdatePacket packet;
                    packet.id = myClientID;
                    glm::vec3 pos = camera.GetPosition();
                    packet.x = pos.x;
                    packet.y = pos.y - 1.8f; // Ayak pozisyonu (Kamera gozde)
                    packet.z = pos.z;
                    packet.yaw = camera.GetYaw();
                    packet.pitch = camera.GetPitch();
                    
                    network.SendPlayerUpdate(packet);
                    networkTimer = 0.0f;
                }
            }

            // TAB tusu ile Editor ac/kapa (3 saniye bekleme suresi)
            if (input->IsKeyPressed(GLFW_KEY_TAB) && tabCooldown <= 0.0f) {
                tabCooldown = 3.0f;
                bool newState = !editor.IsEnabled();
                editor.SetEnabled(newState);
                // Editor acikken de imlec serbest olsun
                if (newState) {
                    input->SetCursorMode(GLFW_CURSOR_NORMAL);
                } else {
                    input->SetCursorMode(GLFW_CURSOR_DISABLED);
                }
            }

            // FPS kontrolcusu guncellemesi
            fpsController.Update(input, &scene, deltaTime);

            // Silah sistemi guncellemesi (mermi olusturma ile)
            if (input->IsMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT) && !editor.IsEnabled()) {
                if (weaponSystem.TryShoot(weapon, player, &scene, &camera, &projectileSystem)) {
                    // Paketi Gonder
                    PlayerShootPacket packet;
                    packet.id = myClientID;
                    glm::vec3 pos = camera.GetPosition();
                    glm::vec3 dir = camera.GetFront();
                    packet.originX = pos.x;
                    packet.originY = pos.y;
                    packet.originZ = pos.z;
                    packet.dirX = dir.x;
                    packet.dirY = dir.y;
                    packet.dirZ = dir.z;
                    packet.weaponType = (int)weapon->type;
                    
                    network.SendPlayerShoot(packet);
                }
            }
            
            weaponSystem.Update(player, input, &scene, &camera, &projectileSystem, deltaTime);
            weaponSystem.UpdateRecoil(weapon, &camera, deltaTime);

            // Mermi sistemi guncellemesi
            projectileSystem.Update(&scene, deltaTime);
            
            // Oyuncu Olum Kontrolu
            if (playerHealthComp->current <= 0.0f) {
                // Yeniden dogus mantigi veya Oyun Bitti
                // Simdilik sadece sifirla
                playerHealthComp->current = 100.0f;
                camera.SetPosition(glm::vec3(0.0f, 2.0f, 0.0f));
                std::cout << "YOU DIED! Respawning..." << std::endl;
            }
        }

        // 3D sahne cizimi
        renderer->BeginFrame();
        
        // Gokyuzu Kutusu Cizimi
        skybox.Draw(camera, (float)config.windowWidth / (float)config.windowHeight);

        renderSystem.Update(deltaTime);
        
        // Editor Arayuzu (ImGui)
        editor.BeginFrame();
        
        // Duraklatma Menusu Mantigi
        if (isPaused) {
            ImGui::SetNextWindowPos(ImVec2(window->GetWidth() * 0.5f, window->GetHeight() * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(400, 500));
            ImGui::Begin("Pause Menu", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            
            if (ImGui::Button("Resume Game", ImVec2(-1, 40))) {
                isPaused = false;
                input->SetCursorMode(GLFW_CURSOR_DISABLED);
            }

            ImGui::Separator();
            ImGui::Text("Audio Settings");
            
            float masterVol = AudioSystem::Get().GetMasterVolume();
            if (ImGui::SliderFloat("Master Volume", &masterVol, 0.0f, 1.0f)) {
                AudioSystem::Get().SetMasterVolume(masterVol);
            }

            float musicVol = AudioSystem::Get().GetMusicVolume();
            if (ImGui::SliderFloat("Music Volume", &musicVol, 0.0f, 1.0f)) {
                AudioSystem::Get().SetMusicVolume(musicVol);
            }

            float sfxVol = AudioSystem::Get().GetSFXVolume();
            if (ImGui::SliderFloat("SFX Volume", &sfxVol, 0.0f, 1.0f)) {
                AudioSystem::Get().SetSFXVolume(sfxVol);
            }

            ImGui::Separator();
            ImGui::Text("Graphics");
            
            bool fullscreen = window->IsFullscreen();
            if (ImGui::Checkbox("Fullscreen", &fullscreen)) {
                window->SetFullscreen(fullscreen);
            }

            ImGui::Separator();
            ImGui::Text("Controls");

            float sensitivity = fpsController.GetMouseSensitivity();
            if (ImGui::SliderFloat("Mouse Sensitivity", &sensitivity, 0.01f, 1.0f)) {
                fpsController.SetMouseSensitivity(sensitivity);
            }

            ImGui::Text("Key Bindings:");
            KeyBindings& bindings = fpsController.GetKeyBindings();

            auto KeyButton = [&](const char* label, int& key) {
                std::string btnLabel = std::string(label) + ": " + (isKeyBindingMode && keyBindingTarget == &key ? "Press Key..." : std::to_string(key));
                if (ImGui::Button(btnLabel.c_str(), ImVec2(-1, 30))) {
                    isKeyBindingMode = true;
                    keyBindingTarget = &key;
                    keyBindingName = label;
                }
            };

            KeyButton("Forward", bindings.forward);
            KeyButton("Backward", bindings.backward);
            KeyButton("Left", bindings.left);
            KeyButton("Right", bindings.right);
            KeyButton("Jump", bindings.jump);
            KeyButton("Sprint", bindings.sprint);

            ImGui::Separator();
            if (ImGui::CollapsingHeader("Multiplayer")) {
                static int serverPort = 12345;
                static char ipBuffer[128] = "127.0.0.1";

                if (!network.IsConnected()) {
                    ImGui::InputText("IP Address", ipBuffer, IM_ARRAYSIZE(ipBuffer));
                    ImGui::InputInt("Port", &serverPort);
                    
                    if (ImGui::Button("Host Server", ImVec2(-1, 30))) {
                        network.StartServer(serverPort);
                    }
                    if (ImGui::Button("Connect", ImVec2(-1, 30))) {
                        network.Connect(ipBuffer, serverPort);
                    }
                } else {
                    ImGui::TextColored(ImVec4(0, 1, 0, 1), "Status: %s", network.IsServer() ? "Hosting" : "Connected");
                    ImGui::Text("My ID: %u", myClientID);
                    ImGui::Text("Remote Players: %d", (int)remotePlayers.size());
                    
                    if (ImGui::Button("Disconnect", ImVec2(-1, 30))) {
                        network.Shutdown();
                        network.Init(); // Re-init
                        remotePlayers.clear();
                    }
                }
            }

            ImGui::Separator();
            if (ImGui::Button("Exit Game", ImVec2(-1, 40))) {
                break;
            }

            ImGui::End();
        } else {
             // Isin izleme mantigi
             // ...
        }

        // Editor Vurgusu icin Isin Izleme
        Entity* lookedAtEntity = nullptr;
        float closestDist = 1000.0f; // Max ray distance
        glm::vec3 rayOrigin = camera.GetPosition();
        glm::vec3 rayDir = camera.GetFront();

        for (const auto& entityPtr : scene.GetEntities()) {
            Entity* entity = entityPtr.get();
            auto* collider = entity->GetComponent<BoxCollider>();
            auto* transform = entity->GetComponent<Transform>();

            if (collider && transform) {
                // AABB Hesapla
                glm::vec3 halfSize = collider->size * transform->scale * 0.5f;
                glm::vec3 boxMin = transform->position - halfSize;
                glm::vec3 boxMax = transform->position + halfSize;

                // Isin-AABB Kesisimi (Slab Yontemi)
                float tMin = (boxMin.x - rayOrigin.x) / rayDir.x;
                float tMax = (boxMax.x - rayOrigin.x) / rayDir.x;

                if (tMin > tMax) std::swap(tMin, tMax);

                float tyMin = (boxMin.y - rayOrigin.y) / rayDir.y;
                float tyMax = (boxMax.y - rayOrigin.y) / rayDir.y;

                if (tyMin > tyMax) std::swap(tyMin, tyMax);

                if ((tMin > tyMax) || (tyMin > tMax)) continue;

                if (tyMin > tMin) tMin = tyMin;
                if (tyMax < tMax) tMax = tyMax;

                float tzMin = (boxMin.z - rayOrigin.z) / rayDir.z;
                float tzMax = (boxMax.z - rayOrigin.z) / rayDir.z;

                if (tzMin > tzMax) std::swap(tzMin, tzMax);

                if ((tMin > tzMax) || (tzMin > tMax)) continue;

                if (tzMin > tMin) tMin = tzMin;
                if (tzMax < tMax) tMax = tzMax;

                // Gecerli kesisim ve oncekinden daha yakin mi kontrol et
                if (tMin > 0 && tMin < closestDist) {
                    closestDist = tMin;
                    lookedAtEntity = entity;
                }
            }
        }
        editor.SetLookedAtEntity(lookedAtEntity);

        editor.Update(&scene, deltaTime, window->GetFPS());
        
        // HUD Cizimi (2D katman) - Sadece oyun sirasinda
        if (!isPaused) {
            hudRenderer.BeginHUD();
            
            // Nisangah
            hudRenderer.DrawCrosshair(20.0f, glm::vec4(0.0f, 1.0f, 0.0f, 0.8f));
            
            // Mermi sayaci (sag alt)
            hudRenderer.DrawAmmoCounter(weapon->stats.currentMag, weapon->stats.magSize, 1680.0f, 50.0f);
            
            // Can bari (sol alt)
            hudRenderer.DrawHealthBar(playerHealthComp->current, playerHealthComp->max, 40.0f, 50.0f, 300.0f, 30.0f);
            
            hudRenderer.EndHUD();
        }
        
        editor.EndFrame();
        
        // Dev Console Cizimi (En ust katman)
        DevConsole::Get().Render();
        
        renderer->EndFrame();

        // Pencere guncellemesi
        window->Update();

        // FPS bilgisini goster (1 saniyede bir)
        static float fpsTimer = 0.0f;
        fpsTimer += deltaTime;
        if (fpsTimer >= 1.0f) {
            std::cout << "FPS: " << window->GetFPS() 
                      << " | Frame Time: " << (deltaTime * 1000.0f) << "ms"
                      << " | Camera Pos: (" << camera.GetPosition().x << ", " 
                      << camera.GetPosition().y << ", " << camera.GetPosition().z << ")"
                      << std::endl;
            fpsTimer = 0.0f;
        }
    }

    // Ag Temizligi
    network.Shutdown();

    // Temizlik
    editor.Shutdown();
    hudRenderer.Shutdown();
    renderSystem.Shutdown();
    AudioSystem::Get().Shutdown();
    
    // Modelleri temizle
    for (auto& entity : scene.GetEntities()) {
        auto* meshRenderer = entity->GetComponent<MeshRenderer>();
        if (meshRenderer && meshRenderer->mesh) {
            delete meshRenderer->mesh;
            meshRenderer->mesh = nullptr;
        }
    }

    engine.Shutdown();

    return 0;
}
