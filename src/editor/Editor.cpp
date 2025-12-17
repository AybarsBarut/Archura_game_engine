#include "Editor.h"
#include "../core/Window.h"
#include "../ecs/Entity.h"
#include "../ecs/Component.h"
#include "../game/Weapon.h"
#include "../game/Projectile.h"
#include "../rendering/Texture.h"
#include "../rendering/Mesh.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../rendering/Camera.h"
#include "../core/ProjectSerializer.h"

namespace Archura {

Editor::Editor()
    : m_Enabled(true)
    , m_SelectedEntity(nullptr)
    , m_Window(nullptr)
{
}

Editor::~Editor() {
    Shutdown();
}

bool Editor::Init(Window* window) {
    m_Window = window;

    // Ciktilari konsola yonlendir
    m_NewCoutBuf = std::make_unique<EditorStreamBuf>(this);
    m_OldCoutBuf = std::cout.rdbuf(m_NewCoutBuf.get());

    // Proje dizinini baslat
    m_BaseProjectDir = std::filesystem::current_path();
    if (std::filesystem::exists(m_BaseProjectDir / "assets")) {
        m_BaseProjectDir /= "assets";
    }
    m_CurrentProjectDir = m_BaseProjectDir;

    // std::cout << "Editor Initialized (UI Logic only)." << std::endl;
    return true;
}

void Editor::Shutdown() {
    // Ciktilari geri yukle
    if (m_OldCoutBuf) {
        std::cout.rdbuf(m_OldCoutBuf);
    }
}



bool Editor::WantCaptureMouse() const {
    if (!m_Enabled) return false;
    // ImGui baglami olusturulmamissa guvenli cikis
    if (!ImGui::GetCurrentContext()) return false;
    return ImGui::GetIO().WantCaptureMouse;
}



void Editor::BeginDockSpace() {
    if (!m_Enabled) return;
    // Docking disabled: ImGui Docking branch not detected
    // ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthroughCentralNode;
    // ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockspace_flags);
}

void Editor::DrawEditorUI() {
    if (!m_Enabled) return;
    SetupLayout();
}

void Editor::SetupLayout() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    float menuBarHeight = ImGui::GetFrameHeight();
    
    ImVec2 workPos = viewport->WorkPos;
    ImVec2 workSize = viewport->WorkSize; 

    // Yerlesim ayari
    float leftPanelWidth = 300.0f;
    float rightPanelWidth = 300.0f;
    float bottomPanelHeight = 250.0f;

    // 1. Arac Cubugu (Kaldirildi)
    // ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + menuBarHeight));
    // ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, toolbarHeight));

    // 2. Hiyerarsi (Sol)
    if (m_ShowSceneHierarchy) {
        ImGui::SetNextWindowPos(workPos, ImGuiCond_FirstUseEver); // Konumu sadece ilk seferde ayarla
        ImGui::SetNextWindowSize(ImVec2(leftPanelWidth, workSize.y - bottomPanelHeight), ImGuiCond_FirstUseEver);
    }

    // 3. Denetci (Sag)
    if (m_ShowInspector) {
        ImGui::SetNextWindowPos(ImVec2(workPos.x + workSize.x - rightPanelWidth, workPos.y), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(rightPanelWidth, workSize.y), ImGuiCond_FirstUseEver);
    }

    // 4. Proje/Konsol (Alt)
    if (m_ShowProjectPanel || m_ShowConsole) {
        ImGui::SetNextWindowPos(ImVec2(workPos.x, workPos.y + workSize.y - bottomPanelHeight), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(workSize.x - (m_ShowInspector ? rightPanelWidth : 0), bottomPanelHeight), ImGuiCond_FirstUseEver);
    }
}

void Editor::DrawMenuBar(Scene* scene) {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Scene", "Ctrl+N")) {}
            if (ImGui::MenuItem("Open Scene", "Ctrl+O")) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Save Project", "Ctrl+S")) {
                if (scene) {
                    ProjectConfig config = { "ArchuraGame", "1.0", "MainScene" };
                    // Ensure directory exists
                    std::filesystem::create_directories("games/ArchuraGame");
                    ProjectSerializer::SaveProject("games/ArchuraGame/project.gameproj", config, scene);
                    Log("Project saved to games/ArchuraGame/project.gameproj");
                }
            }
            if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Alt+F4")) {}
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "Ctrl+Z")) {}
            if (ImGui::MenuItem("Redo", "Ctrl+Y")) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "Ctrl+X")) {}
            if (ImGui::MenuItem("Copy", "Ctrl+C")) {}
            if (ImGui::MenuItem("Paste", "Ctrl+V")) {}
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Selection")) {
            if (ImGui::MenuItem("Select All", "Ctrl+A")) {}
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Scene Hierarchy", nullptr, &m_ShowSceneHierarchy);
            ImGui::MenuItem("Inspector", nullptr, &m_ShowInspector);
            ImGui::MenuItem("Project", nullptr, &m_ShowProjectPanel);
            ImGui::MenuItem("Console", nullptr, &m_ShowConsole);
            ImGui::Separator();
            ImGui::MenuItem("Performance Metrics", nullptr, &m_ShowPerformance);
            ImGui::MenuItem("ImGui Demo Window", nullptr, &m_ShowDemoWindow);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Go")) {
            if (ImGui::MenuItem("Go to File...", "Ctrl+P")) {}
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Run")) {
            if (ImGui::MenuItem("Start Debugging", "F5")) {}
            if (ImGui::MenuItem("Run Without Debugging", "Ctrl+F5")) {}
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Terminal")) {
            if (ImGui::MenuItem("New Terminal")) { m_ShowConsole = true; }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Entity")) {
            if (ImGui::BeginMenu("Load Model (.obj / .fbx)")) {
                std::string path = "assets/models";
                if (std::filesystem::exists(path)) {
                    for (const auto& entry : std::filesystem::directory_iterator(path)) {
                        std::string ext = entry.path().extension().string();
                        // Kucuk harfe cevir (windows genelde case-insensitive ama garanti olsun)
                        // std::transform... (ne gerek var, basit kontrol)
                        
                        bool isObj = (ext == ".obj" || ext == ".OBJ");
                        bool isFbx = (ext == ".fbx" || ext == ".FBX");

                        if (isObj || isFbx) {
                            std::string filename = entry.path().filename().string();
                            
                            // Gorsel olarak ayirt etmek icin ikon veya yazi eklenebilir
                            std::string label = filename + (isFbx ? " [FBX-WIP]" : " [OBJ]");

                            if (ImGui::MenuItem(label.c_str())) {
                                if (scene) {
                                    Entity* e = scene->CreateEntity(entry.path().stem().string());
                                    
                                    // Transform
                                    auto* t = e->GetComponent<Transform>();
                                    t->position = glm::vec3(0, 5, 0); 
                                    
                                    // Mesh
                                    auto* mr = e->AddComponent<MeshRenderer>();
                                    if (isObj) {
                                        mr->mesh = Mesh::LoadFromOBJ(entry.path().string());
                                        mr->color = glm::vec3(1.0f);
                                    } else {
                                        mr->mesh = Mesh::LoadFromFBX(entry.path().string());
                                        mr->color = glm::vec3(1.0f, 0.0f, 0.0f); // Kirmizi (Placeholder oldugunu belli etmek icin)
                                    }
                                    
                                    // Collider
                                    e->AddComponent<BoxCollider>()->size = glm::vec3(1.0f);
                                    
                                    Log("Spawned Model: " + filename);
                                }
                            }
                        }
                    }
                } else {
                    ImGui::TextDisabled("No 'assets/models' folder found");
                }
                ImGui::EndMenu();
            }
            // Other entity spawns
            if (ImGui::MenuItem("Cube")) {
                if (scene) {
                    Entity* e = scene->CreateEntity("Cube");
                    e->AddComponent<MeshRenderer>()->mesh = Mesh::CreateCube();
                    e->AddComponent<BoxCollider>();
                    e->GetComponent<Transform>()->position = glm::vec3(0, 5, 0);
                }
            }
            if (ImGui::MenuItem("Light")) {
                 if (scene) SpawnEntity(scene, "Light");
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About Archura Engine")) {}
            ImGui::EndMenu();
        }

        // Play / Stop Button in Menu Bar
        ImGui::Separator();
        /*
        if (m_IsGameRunning) {
            if (ImGui::MenuItem("STOP [Esc]")) {
                m_IsGameRunning = false;
                Log("Game Stopped.");
            }
        } else {
            if (ImGui::MenuItem("PLAY [F5]")) {
                m_IsGameRunning = true;
                Log("Game Started.");
            }
        }
        */

        ImGui::EndMainMenuBar();
    }
}



void Editor::DrawOverlay(Scene* scene, Camera* camera) {
    const float DISTANCE = 10.0f;
    static int corner = 1; // Top-right
    ImGuiIO& io = ImGui::GetIO();
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    
    if (corner != -1) {
        window_flags |= ImGuiWindowFlags_NoMove;
        ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE + 20.0f); // +20 for menu bar
        ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    }
    
    ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
    if (ImGui::Begin("Overlay", nullptr, window_flags)) {
        ImGui::Text("Archura Engine Dev");
        ImGui::Separator();
        
        if (camera) {
            glm::vec3 pos = camera->GetPosition();
            ImGui::Text("Pos: (%.1f, %.1f, %.1f)", pos.x, pos.y, pos.z);
            ImGui::Text("Rot: (%.1f, %.1f, 0.0)", camera->GetPitch(), camera->GetYaw());
        }
        
        ImGui::Text("FPS: %.1f", io.Framerate);
    }
    ImGui::End();
}

void Editor::DrawSceneHierarchy(Scene* scene) {
    ImGui::Begin("Scene Hierarchy");

    if (!scene) {
        ImGui::Text("No active scene");
        ImGui::End();
        return;
    }

    // Varlik listesi
    const auto& entities = scene->GetEntities();
    
    ImGui::Text("Entities: %zu", entities.size());
    
    // + Entity Button
    if (ImGui::Button("+ Entity", ImVec2(-1, 0))) {
        ImGui::OpenPopup("AddEntityPopup");
    }

    if (ImGui::BeginPopup("AddEntityPopup")) {
        // Primitives
        if (ImGui::BeginMenu("Primitives")) {
            if (ImGui::MenuItem("Cube")) SpawnEntity(scene, "Cube");
            if (ImGui::MenuItem("Sphere")) SpawnEntity(scene, "Sphere");
            if (ImGui::MenuItem("Capsule")) SpawnEntity(scene, "Capsule");
            if (ImGui::MenuItem("Stairs")) SpawnEntity(scene, "Stairs");
            if (ImGui::MenuItem("Ramp")) SpawnEntity(scene, "Ramp");
            if (ImGui::MenuItem("Light")) SpawnEntity(scene, "Light");
            ImGui::EndMenu();
        }

        ImGui::Separator();
        
        // Models from assets/models
        ImGui::TextDisabled("Models (assets/models)");
        std::string modelsPath = "assets/models";
        if (std::filesystem::exists(modelsPath)) {
            DrawModelDirectory(scene, modelsPath);
        } else {
            ImGui::TextDisabled("Folder not found");
        }

        ImGui::EndPopup();
    }

    ImGui::Separator();

    for (const auto& entityPtr : entities) {
        Entity* entity = entityPtr.get();
        
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        if (entity == m_SelectedEntity) {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        bool isLookedAt = (entity == m_LookedAtEntity);
        if (isLookedAt) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f)); // Yesil
        }

        ImGui::TreeNodeEx(entity, flags, "%s (ID: %u)%s", 
                         entity->GetName().c_str(), 
                         entity->GetID(),
                         isLookedAt ? " <--" : "");

        if (isLookedAt) {
            ImGui::PopStyleColor();
        }

        if (ImGui::IsItemClicked()) {
            m_SelectedEntity = entity;
        }
    }

    ImGui::End();
}

void Editor::DrawInspector(Scene* scene) {
    ImGui::Begin("Inspector");

    if (!m_SelectedEntity) {
        ImGui::Text("No entity selected");
        ImGui::End();
        return;
    }

    // Varlik bilgileri
    // ImGui::Text("Entity: %s", m_SelectedEntity->GetName().c_str());
    
    // Entity Renaming
    static char nameBuf[128] = "";
    // Secili entity degistiginde buffer'i guncellemek gerekir. 
    // Ancak her frame'de burasi calisiyor. 
    // Basit bir yaklasim: Her zaman degil, ID degistiginde bufferi yenile.
    // Veya ImGui ID sistemiyle cakismamak icin gecici buffer kullan.
    
    std::string currentName = m_SelectedEntity->GetName();
    if (m_SelectedEntity->GetID() != m_CachedEntityID) {
        strncpy(nameBuf, currentName.c_str(), sizeof(nameBuf));
        nameBuf[sizeof(nameBuf) - 1] = '\0';
        m_CachedEntityID = m_SelectedEntity->GetID();
    }

    if (ImGui::InputText("Name", nameBuf, sizeof(nameBuf), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) {
        if (strlen(nameBuf) > 0) {
            m_SelectedEntity->SetName(nameBuf);
        }
    }
    // Enter'a basilmasa bile her karakterde guncellemek istersek:
    // Ancak InputText aktifken surekli SetName cagirmak da mumkun.
    // Simdilik EnterReturnsTrue ile yaptik ama kullanici deneyimi icin:
    if (ImGui::IsItemDeactivatedAfterEdit()) {
         if (strlen(nameBuf) > 0) {
            m_SelectedEntity->SetName(nameBuf);
        }
    }

    ImGui::Text("ID: %u", m_SelectedEntity->GetID());
    ImGui::Separator();

    // Donusum bileseni
    auto* transform = m_SelectedEntity->GetComponent<Transform>();
    if (transform && ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::DragFloat3("Position", &transform->position.x, 0.1f);
        ImGui::DragFloat3("Rotation", &transform->rotation.x, 1.0f);
        ImGui::DragFloat3("Scale", &transform->scale.x, 0.1f);
    }

    // Model isleyici bileseni
    auto* meshRenderer = m_SelectedEntity->GetComponent<MeshRenderer>();
    if (meshRenderer && ImGui::CollapsingHeader("Mesh Renderer")) {
        ImGui::Text("Mesh: %s", meshRenderer->mesh ? "Loaded" : "None");
        ImGui::ColorEdit3("Color", &meshRenderer->color.x);
    }

    // Kaplama / Doku
    if (meshRenderer && ImGui::CollapsingHeader("Skin / Texture")) {
        // Dokulari tara
        static std::vector<std::string> textureFiles;
        static bool filesLoaded = false;
        
        if (!filesLoaded || ImGui::Button("Refresh List")) {
            textureFiles.clear();
            std::string texturesDir = "assets/textures";
            if (std::filesystem::exists(texturesDir)) {
                for (const auto& entry : std::filesystem::directory_iterator(texturesDir)) {
                    if (entry.is_regular_file()) {
                        std::string ext = entry.path().extension().string();
                        // Uzanti kontrolu
                        if (ext == ".jpg" || ext == ".png" || ext == ".tga" || ext == ".bmp" || ext == ".jpeg") {
                            textureFiles.push_back(entry.path().filename().string());
                        }
                    }
                }
            }
            filesLoaded = true;
        }

        static int selectedTextureIdx = -1;
        if (ImGui::BeginListBox("Available Textures")) {
            for (int i = 0; i < textureFiles.size(); i++) {
                const bool isSelected = (selectedTextureIdx == i);
                if (ImGui::Selectable(textureFiles[i].c_str(), isSelected)) {
                    selectedTextureIdx = i;
                    // Dokuyu yukle
                    std::string pathStr = "assets/textures/" + textureFiles[i];
                    std::string name = std::filesystem::path(pathStr).stem().string();
                    Texture* tex = TextureManager::Get().Load(name, pathStr);
                    if (tex) {
                        meshRenderer->texture = tex;
                        Log("Texture loaded: " + pathStr);
                    }
                }

                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndListBox();
        }

        if (meshRenderer->texture) {
            ImGui::Text("Current ID: %u", meshRenderer->texture->GetID());
            ImGui::Image((void*)(intptr_t)meshRenderer->texture->GetID(), ImVec2(100, 100));
            
            if (ImGui::Button("Remove Texture")) {
                meshRenderer->texture = nullptr;
            }
        }
    }

    // Kutu Carpistirici
    auto* boxCollider = m_SelectedEntity->GetComponent<BoxCollider>();
    if (boxCollider && ImGui::CollapsingHeader("Box Collider")) {
        ImGui::DragFloat3("Size", &boxCollider->size.x, 0.1f);
        ImGui::DragFloat3("Center", &boxCollider->center.x, 0.1f);
        ImGui::Checkbox("Is Trigger", &boxCollider->isTrigger);
    }

    // Silah bileseni
    auto* weapon = m_SelectedEntity->GetComponent<Weapon>();
    if (weapon && ImGui::CollapsingHeader("Weapon")) {
        const char* weaponTypes[] = { "Rifle", "Pistol", "Knife", "Grenade", "Shotgun", "Sniper" };
        int currentType = (int)weapon->type;
        if (ImGui::Combo("Type", &currentType, weaponTypes, 6)) {
            weapon->SwitchWeapon((Weapon::WeaponType)currentType);
        }
        
        ImGui::DragFloat("Damage", &weapon->stats.damage, 1.0f, 0.0f, 200.0f);
        ImGui::DragFloat("Fire Rate", &weapon->stats.fireRate, 0.01f, 0.01f, 5.0f);
        ImGui::DragFloat("Range", &weapon->stats.range, 1.0f, 10.0f, 500.0f);
        // Isabet orani simdilik kaldirildi
        
        ImGui::Separator();
        ImGui::Text("Ammo:");
        ImGui::Text("  Magazine: %d / %d", weapon->stats.currentMag, weapon->stats.magSize);
        ImGui::Text("  Total: %d", weapon->stats.totalAmmo);
        ImGui::Text("  Reloading: %s", weapon->isReloading ? "Yes" : "No");
    }

    // Mermi bileseni
    auto* projectile = m_SelectedEntity->GetComponent<Projectile>();
    if (projectile && ImGui::CollapsingHeader("Projectile")) {
        ImGui::Text("Speed: %.2f", projectile->speed);
        ImGui::Text("Damage: %.2f", projectile->damage);
        ImGui::Text("Lifetime: %.2f", projectile->lifetime);
        ImGui::Text("Has Hit: %s", projectile->hasHit ? "Yes" : "No");
        ImGui::DragFloat("Gravity", &projectile->gravity, 0.1f, -20.0f, 0.0f);
    }

    // Isik Bileseni
    auto* lightComp = m_SelectedEntity->GetComponent<LightComponent>();
    if (lightComp && ImGui::CollapsingHeader("Light Component")) {
        const char* lightTypes[] = { "Directional", "Point", "Circle (Ambient)" };
        int currentType = (int)lightComp->type;
        if (ImGui::Combo("Type", &currentType, lightTypes, 3)) {
            lightComp->type = (LightComponent::Type)currentType;
        }

        ImGui::ColorEdit3("Color", &lightComp->color.x);
        ImGui::DragFloat("Intensity", &lightComp->intensity, 0.1f, 0.0f, 100.0f);
        
        if (lightComp->type == LightComponent::Type::Point) {
            ImGui::DragFloat("Range", &lightComp->range, 0.5f, 0.0f, 1000.0f);
        }
    }

    ImGui::Separator();
    ImGui::Spacing();
    
    // Delete Button (Red)
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.1f, 0.1f, 1.0f));
    
    if (ImGui::Button("Delete Entity", ImVec2(-1, 0))) {
        if (scene) {
            scene->DestroyEntity(m_SelectedEntity->GetID());
            m_SelectedEntity = nullptr;
        }
    }
    
    ImGui::PopStyleColor(3);

    ImGui::End();
}

void Editor::DrawPerformanceMetrics(float deltaTime, float fps) {
    ImGui::Begin("Performance");

    ImGui::Text("FPS: %.1f", fps);
    ImGui::Text("Frame Time: %.3f ms", deltaTime * 1000.0f);
    
    ImGui::Separator();
    
    // Kare suresi grafigi
    static float frameTimes[90] = {};
    static int frameTimeIdx = 0;
    frameTimes[frameTimeIdx] = deltaTime * 1000.0f;
    frameTimeIdx = (frameTimeIdx + 1) % 90;
    
    ImGui::PlotLines("Frame Time (ms)", frameTimes, 90, 0, nullptr, 0.0f, 33.0f, ImVec2(0, 80));

    // Bellek kullanimi (tahmini)
    ImGui::Separator();
    ImGui::Text("Memory (Estimate):");
    ImGui::Text("  Engine: ~930 MB");

    ImGui::End();
}

void Editor::DrawDemoWindow() {
    ImGui::ShowDemoWindow(&m_ShowDemoWindow);
}

void Editor::DrawToolbar() {
    // Arac cubugu kullanici istegiyle kaldirildi
}

void Editor::DrawProjectPanel() {
    ImGui::Begin("Project");

    if (m_CurrentProjectDir != m_BaseProjectDir) {
        if (ImGui::Button("..")) {
            m_CurrentProjectDir = m_CurrentProjectDir.parent_path();
        }
    }

    // Gecerli yol
    ImGui::Text("Path: %s", m_CurrentProjectDir.string().c_str());
    ImGui::Separator();

    // Klasorleri listele
    for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentProjectDir)) {
        const auto& path = directoryEntry.path();
        auto relativePath = std::filesystem::relative(path, m_BaseProjectDir);
        std::string filenameString = path.filename().string();

        if (directoryEntry.is_directory()) {
            if (ImGui::Selectable((filenameString + "/").c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
                if (ImGui::IsMouseDoubleClicked(0)) {
                    m_CurrentProjectDir /= path.filename();
                }
            }
        }
    }

    // Dosyalari listele
    for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentProjectDir)) {
        const auto& path = directoryEntry.path();
        std::string filenameString = path.filename().string();

        if (!directoryEntry.is_directory()) {
            if (ImGui::Selectable(filenameString.c_str())) {
                // Dosya secimi
            }
        }   
    }

    ImGui::End();
}

void Editor::DrawConsolePanel() {
    ImGui::Begin("Console");

    if (ImGui::Button("Clear")) {
        ClearLogs();
    }
    ImGui::SameLine();
    ImGui::Text("Logs: %zu", m_ConsoleLogs.size());
    ImGui::Separator();

    // Alt kisimda giris icin yer ayir
    float footerHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footerHeight), false, ImGuiWindowFlags_HorizontalScrollbar);
    for (const auto& log : m_ConsoleLogs) {
        ImGui::TextUnformatted(log.c_str());
    }
    
    // Otomatik kaydirma
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();

    ImGui::Separator();
    
    // Komut girisi
    ImGui::PushItemWidth(-1);
    if (ImGui::InputText("##Input", m_InputBuf, IM_ARRAYSIZE(m_InputBuf), ImGuiInputTextFlags_EnterReturnsTrue)) {
        ExecuteCommand(m_InputBuf);
        m_InputBuf[0] = '\0';
        ImGui::SetKeyboardFocusHere(-1); // Keep focus
    }
    ImGui::PopItemWidth();

    ImGui::End();
}

void Editor::ExecuteCommand(const char* command) {
    Log("> " + std::string(command));
    
    std::string cmd = command;
    if (cmd == "clear") {
        ClearLogs();
    } else if (cmd == "help") {
        Log("Available commands: clear, help");
    } else {
        Log("Unknown command: " + cmd);
    }
}

void Editor::SpawnEntity(Scene* scene, const std::string& type, const std::string& path) {
    if (!scene) return;

    static int entityCounter = 0;
    std::string name = type + "_" + std::to_string(++entityCounter);
    
    Entity* entity = scene->CreateEntity(name);
    auto* transform = entity->GetComponent<Transform>();
    transform->position = m_SpawnPosition;

    auto* meshRenderer = entity->AddComponent<MeshRenderer>();
    meshRenderer->color = glm::vec3(1.0f); // White default

    auto* collider = entity->AddComponent<BoxCollider>();
    collider->size = glm::vec3(1.0f);

    if (type == "Cube") {
        meshRenderer->mesh = Mesh::CreateCube(1.0f);
    } else if (type == "Sphere") {
        meshRenderer->mesh = Mesh::CreateSphere(0.5f); // Radius 0.5 = Diameter 1.0
    } else if (type == "Capsule") {
        meshRenderer->mesh = Mesh::CreateCapsule(0.5f, 2.0f);
        collider->size = glm::vec3(1.0f, 2.0f, 1.0f);
    } else if (type == "Stairs") {
        meshRenderer->mesh = Mesh::CreateStairs(1.0f, 1.0f, 2.0f, 5); // 5 basamak
        collider->size = glm::vec3(1.0f, 1.0f, 2.0f);
    } else if (type == "Ramp") {
        meshRenderer->mesh = Mesh::CreateRamp(1.0f, 1.0f, 2.0f);
        collider->size = glm::vec3(1.0f, 1.0f, 2.0f);
    } else if (type == "Light") {
        // Isik kaynagi icin component ekle
        entity->AddComponent<LightComponent>();
        
        // Gorsel bir temsil (Editor-only? Simdilik sari kucuk bir kup)
        // Oyun icinde gorunmesin istiyorsak MeshRenderer eklemeyebiliriz 
        // Veya "Editor Only" flag lazim. 
        // Kullanici "gorunur olmasin" dedi. O yuzden MeshRenderer eklemeyelim veya
        // debug modunda cizdirelim. DebugMesh olmadigi icin simdilik bos birakiyoruz.
        // Ama secilmesi icin Collider lazim olabilir veya Hierarchy'den secilecek.
        // Konumunu gormek icin kucuk bir wireframe cizdirilebilir ama simdilik sadece hierarchy.
        
        // MeshRenderer'i kaldir (otomatik eklendi yukarida)
        entity->RemoveComponent<MeshRenderer>();
        // Collider? Secmek icin ise yarar ama fiziksel etkilesim olmamali.
        // Trigger yapalim.
        collider->isTrigger = true;
        collider->size = glm::vec3(0.5f);
        
        // Simdilik kullanici nerede oldugunu bilsin diye ufak bir mesh koyalim mi?
        // "Gorunur olmasin" dedi. O yuzden koymuyoruz.
        // Editor'de ikon cizimi henuz yok.
        
    } else if (type == "Model" && !path.empty()) {
        std::string ext = std::filesystem::path(path).extension().string();
        
        // Basit lowercase
        for (auto & c: ext) c = tolower(c);

        if (ext == ".obj") {
            meshRenderer->mesh = Mesh::LoadFromOBJ(path);
        } else if (ext == ".fbx") {
            meshRenderer->mesh = Mesh::LoadFromFBX(path);
        } else if (ext == ".glb" || ext == ".gltf") {
            Log("[WARN] GLB/GLTF loading is not yet implemented natively.");
            // Bir placeholder mesh (orn. Kure) koyalim ki bos kalmasin
            meshRenderer->mesh = Mesh::CreateSphere(0.5f);
            meshRenderer->color = glm::vec3(1.0f, 0.0f, 1.0f); // Magenta uyari rengi
        }
        
        // Model adini entity adi yap (eger override edilmediyse)
        if (entity->GetName().find("Model_") == 0) {
            entity->SetName(std::filesystem::path(path).stem().string());
        }
        
        Log("Spawned Model from: " + path);
    }

    Log("Spawned " + type + " at " + std::to_string(m_SpawnPosition.x) + ", " + std::to_string(m_SpawnPosition.y) + ", " + std::to_string(m_SpawnPosition.z));
    m_SelectedEntity = entity;
}

void Editor::DrawModelDirectory(Scene* scene, const std::filesystem::path& directory) {
    // Basit bir siralama icin once klasorleri sonra dosyalari toplayalim
    std::vector<std::filesystem::directory_entry> directories;
    std::vector<std::filesystem::directory_entry> files;

    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_directory()) {
            directories.push_back(entry);
        } else if (entry.is_regular_file()) {
            files.push_back(entry);
        }
    }

    // Klasorleri ciz
    for (const auto& dir : directories) {
        if (ImGui::BeginMenu(dir.path().filename().string().c_str())) {
            DrawModelDirectory(scene, dir.path());
            ImGui::EndMenu();
        }
    }

    if (!directories.empty() && !files.empty()) {
        ImGui::Separator();
    }

    // Dosyalari ciz
    for (const auto& file : files) {
        std::string ext = file.path().extension().string();
        std::string extLower = ext;
        for (auto & c: extLower) c = tolower(c);

        bool isModel = (extLower == ".obj" || extLower == ".fbx" || 
                        extLower == ".glb" || extLower == ".gltf");

        if (isModel) {
            std::string filename = file.path().filename().string();
            // Dosya ikon veya tur bilgisi eklenebilir
            if (ImGui::MenuItem(filename.c_str())) {
                SpawnEntity(scene, "Model", file.path().string());
            }
        }
    }
}

void Editor::Update(Scene* scene, float deltaTime, float fps) {
    if (m_Enabled) {
        DrawToolbar();
        
        if (m_ShowSceneHierarchy) DrawSceneHierarchy(scene);
        if (m_ShowInspector) DrawInspector(scene);
        if (m_ShowProjectPanel) DrawProjectPanel();
        if (m_ShowConsole) DrawConsolePanel();
        if (m_ShowPerformance) DrawPerformanceMetrics(deltaTime, fps);
        if (m_ShowDemoWindow) DrawDemoWindow();
        
        // Overlay (Pass nullptr for camera for now as we don't have it here)
        DrawOverlay(scene, nullptr);
    }
}



} // namespace Archura
