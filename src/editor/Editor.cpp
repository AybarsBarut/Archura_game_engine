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
    // ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
}

void Editor::EndFrame() {
    if (!m_Enabled) return;

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Editor::Update(Scene* scene, float deltaTime, float fps) {
    if (!m_Enabled) return;

    DrawMenuBar();

    if (m_ShowSceneHierarchy) {
        DrawSceneHierarchy(scene);
    }

    if (m_ShowInspector) {
        DrawInspector();
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

        ImGui::TreeNodeEx(entity, flags, "%s (ID: %u)", 
                         entity->GetName().c_str(), 
                         entity->GetID());

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
        const char* weaponTypes[] = { "Pistol", "Rifle", "Shotgun", "Sniper" };
        int currentType = (int)weapon->type;
        if (ImGui::Combo("Type", &currentType, weaponTypes, 4)) {
            weapon->type = (Weapon::WeaponType)currentType;
        }
        
        ImGui::DragFloat("Damage", &weapon->damage, 1.0f, 0.0f, 200.0f);
        ImGui::DragFloat("Fire Rate", &weapon->fireRate, 0.01f, 0.01f, 5.0f);
        ImGui::DragFloat("Range", &weapon->range, 1.0f, 10.0f, 500.0f);
        ImGui::SliderFloat("Accuracy", &weapon->accuracy, 0.0f, 1.0f);
        
        ImGui::Separator();
        ImGui::Text("Ammo:");
        ImGui::Text("  Magazine: %d / %d", weapon->currentMag, weapon->magSize);
        ImGui::Text("  Total: %d", weapon->totalAmmo);
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

} // namespace Archura
