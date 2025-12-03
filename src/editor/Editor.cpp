#include "Editor.h"
#include "../core/Window.h"
#include "../ecs/Entity.h"
#include "../ecs/Component.h"
#include "../game/Weapon.h"
#include "../game/Projectile.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <iostream>

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

    // ImGui context olustur
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Style
    ImGui::StyleColorsDark();

    // Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window->GetNativeWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");

    std::cout << "ImGui Editor initialized." << std::endl;
    return true;
}

void Editor::Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Editor::BeginFrame() {
    if (!m_Enabled) return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Dockspace (tum pencereyi kullan)
    // ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthroughCentralNode;
    // ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockspace_flags);

    SetupLayout();
}

void Editor::SetupLayout() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    float menuBarHeight = ImGui::GetFrameHeight();
    float toolbarHeight = 30.0f;
    
    ImVec2 workPos = viewport->WorkPos;
    ImVec2 workSize = viewport->WorkSize;
    workPos.y += toolbarHeight; // Toolbar icin yer ac
    workSize.y -= toolbarHeight;

    // Layout configuration
    float leftPanelWidth = 300.0f;
    float rightPanelWidth = 300.0f;
    float bottomPanelHeight = 250.0f;

    // 1. Toolbar (Top)
    ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y));
    ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, toolbarHeight));

    // 2. Hierarchy (Left)
    if (m_ShowSceneHierarchy) {
        ImGui::SetNextWindowPos(workPos);
        ImGui::SetNextWindowSize(ImVec2(leftPanelWidth, workSize.y - bottomPanelHeight));
    }

    // 3. Inspector (Right)
    if (m_ShowInspector) {
        ImGui::SetNextWindowPos(ImVec2(workPos.x + workSize.x - rightPanelWidth, workPos.y));
        ImGui::SetNextWindowSize(ImVec2(rightPanelWidth, workSize.y));
    }

    // 4. Project/Console (Bottom)
    if (m_ShowProjectPanel || m_ShowConsole) {
        ImGui::SetNextWindowPos(ImVec2(workPos.x, workPos.y + workSize.y - bottomPanelHeight));
        ImGui::SetNextWindowSize(ImVec2(workSize.x - (m_ShowInspector ? rightPanelWidth : 0), bottomPanelHeight));
    }
}

void Editor::EndFrame() {
    if (!m_Enabled) return;

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Editor::Update(Scene* scene, float deltaTime, float fps) {
    if (!m_Enabled) return;

    DrawMenuBar();
    DrawToolbar();

    if (m_ShowSceneHierarchy) {
        DrawSceneHierarchy(scene);
    }

    if (m_ShowInspector) {
        DrawInspector();
    }

    if (m_ShowProjectPanel) {
        DrawProjectPanel();
    }

    if (m_ShowConsole) {
        DrawConsolePanel();
    }

    if (m_ShowPerformance) {
        DrawPerformanceMetrics(deltaTime, fps);
    }

    if (m_ShowDemoWindow) {
        ImGui::ShowDemoWindow(&m_ShowDemoWindow);
    }
}

void Editor::DrawMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Scene")) {
                // TODO: New scene
            }
            if (ImGui::MenuItem("Open Scene")) {
                // TODO: Load scene
            }
            if (ImGui::MenuItem("Save Scene")) {
                // TODO: Save scene
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                // TODO: Exit application
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Scene Hierarchy", nullptr, &m_ShowSceneHierarchy);
            ImGui::MenuItem("Inspector", nullptr, &m_ShowInspector);
            ImGui::MenuItem("Project", nullptr, &m_ShowProjectPanel);
            ImGui::MenuItem("Console", nullptr, &m_ShowConsole);
            ImGui::MenuItem("Performance", nullptr, &m_ShowPerformance);
            ImGui::Separator();
            ImGui::MenuItem("ImGui Demo", nullptr, &m_ShowDemoWindow);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void Editor::DrawSceneHierarchy(Scene* scene) {
    ImGui::Begin("Scene Hierarchy");

    if (!scene) {
        ImGui::Text("No active scene");
        ImGui::End();
        return;
    }

    // Entities listesini goster
    const auto& entities = scene->GetEntities();
    
    ImGui::Text("Entities: %zu", entities.size());
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

void Editor::DrawInspector() {
    ImGui::Begin("Inspector");

    if (!m_SelectedEntity) {
        ImGui::Text("No entity selected");
        ImGui::End();
        return;
    }

    // Entity bilgileri
    ImGui::Text("Entity: %s", m_SelectedEntity->GetName().c_str());
    ImGui::Text("ID: %u", m_SelectedEntity->GetID());
    ImGui::Separator();

    // Transform component
    auto* transform = m_SelectedEntity->GetComponent<Transform>();
    if (transform && ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::DragFloat3("Position", &transform->position.x, 0.1f);
        ImGui::DragFloat3("Rotation", &transform->rotation.x, 1.0f);
        ImGui::DragFloat3("Scale", &transform->scale.x, 0.1f);
    }

    // MeshRenderer component
    auto* meshRenderer = m_SelectedEntity->GetComponent<MeshRenderer>();
    if (meshRenderer && ImGui::CollapsingHeader("Mesh Renderer")) {
        ImGui::Text("Mesh: %s", meshRenderer->mesh ? "Loaded" : "None");
        ImGui::ColorEdit3("Color", &meshRenderer->color.x);
    }

    // Weapon component
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
        // Accuracy removed from stats for now, or add it back if needed
        
        ImGui::Separator();
        ImGui::Text("Ammo:");
        ImGui::Text("  Magazine: %d / %d", weapon->stats.currentMag, weapon->stats.magSize);
        ImGui::Text("  Total: %d", weapon->stats.totalAmmo);
        ImGui::Text("  Reloading: %s", weapon->isReloading ? "Yes" : "No");
    }

    // Projectile component
    auto* projectile = m_SelectedEntity->GetComponent<Projectile>();
    if (projectile && ImGui::CollapsingHeader("Projectile")) {
        ImGui::Text("Speed: %.2f", projectile->speed);
        ImGui::Text("Damage: %.2f", projectile->damage);
        ImGui::Text("Lifetime: %.2f", projectile->lifetime);
        ImGui::Text("Has Hit: %s", projectile->hasHit ? "Yes" : "No");
        ImGui::DragFloat("Gravity", &projectile->gravity, 0.1f, -20.0f, 0.0f);
    }

    ImGui::End();
}

void Editor::DrawPerformanceMetrics(float deltaTime, float fps) {
    ImGui::Begin("Performance");

    ImGui::Text("FPS: %.1f", fps);
    ImGui::Text("Frame Time: %.3f ms", deltaTime * 1000.0f);
    
    ImGui::Separator();
    
    // Frame time graph
    static float frameTimes[90] = {};
    static int frameTimeIdx = 0;
    frameTimes[frameTimeIdx] = deltaTime * 1000.0f;
    frameTimeIdx = (frameTimeIdx + 1) % 90;
    
    ImGui::PlotLines("Frame Time (ms)", frameTimes, 90, 0, nullptr, 0.0f, 33.0f, ImVec2(0, 80));

    // Memory kullanimi (tahmini)
    ImGui::Separator();
    ImGui::Text("Memory (Estimate):");
    ImGui::Text("  Engine: ~930 MB");

    ImGui::End();
}

void Editor::DrawDemoWindow() {
    ImGui::ShowDemoWindow(&m_ShowDemoWindow);
}

void Editor::DrawToolbar() {
    ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    
    // Center the buttons
    float width = ImGui::GetWindowWidth();
    float buttonWidth = 32.0f;
    ImGui::SetCursorPosX((width - buttonWidth * 3) * 0.5f);

    if (ImGui::Button("Play", ImVec2(buttonWidth, 0))) {
        // TODO: Play logic
        Log("Play pressed");
    }
    ImGui::SameLine();
    if (ImGui::Button("Pause", ImVec2(buttonWidth, 0))) {
        // TODO: Pause logic
        Log("Pause pressed");
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop", ImVec2(buttonWidth, 0))) {
        // TODO: Stop logic
        Log("Stop pressed");
    }

    ImGui::End();
}

void Editor::DrawProjectPanel() {
    ImGui::Begin("Project");

    // Simple file list simulation
    static std::vector<std::string> files = {
        "Assets/",
        "  Models/",
        "    character.obj",
        "    cube.obj",
        "  Textures/",
        "    grass.jpg",
        "    wall.jpg",
        "  Scripts/",
        "    PlayerController.cpp"
    };

    // Split view: Tree on left, Content on right
    // For now, just a list
    for (const auto& file : files) {
        if (ImGui::Selectable(file.c_str())) {
            // Select file
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

    ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
    for (const auto& log : m_ConsoleLogs) {
        ImGui::TextUnformatted(log.c_str());
    }
    
    // Auto-scroll
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();
    ImGui::End();
}

} // namespace Archura
