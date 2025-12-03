#pragma once

#include <memory>
#include <string>
#include <vector>

namespace Archura {

class Scene;
class Entity;
class Window;

/**
 * @brief ImGui Editor - Main editor coordinator
 * 
 * Tüm editor panellerini yönetir ve ImGui lifecycle'ı handle eder
 */
class Editor {
public:
    Editor();
    ~Editor();

    bool Init(Window* window);
    void Shutdown();

    void BeginFrame();
    void EndFrame();

    void Update(Scene* scene, float deltaTime, float fps);

    void SetEnabled(bool enabled) { m_Enabled = enabled; }
    bool IsEnabled() const { return m_Enabled; }

    // Selected entity
    Entity* GetSelectedEntity() const { return m_SelectedEntity; }
    void SetSelectedEntity(Entity* entity) { m_SelectedEntity = entity; }

    // Looked at entity (Debug)
    void SetLookedAtEntity(Entity* entity) { m_LookedAtEntity = entity; }

private:
    void SetupLayout();
    void DrawMenuBar();
    void DrawToolbar();
    void DrawSceneHierarchy(Scene* scene);
    void DrawInspector();
    void DrawProjectPanel();
    void DrawConsolePanel();
    void DrawPerformanceMetrics(float deltaTime, float fps);
    void DrawDemoWindow();

public:
    // Console logging
    void Log(const std::string& message) { m_ConsoleLogs.push_back(message); }
    void ClearLogs() { m_ConsoleLogs.clear(); }

private:
    bool m_Enabled = true;
    bool m_ShowDemoWindow = false;
    bool m_ShowSceneHierarchy = true;
    bool m_ShowInspector = true;
    bool m_ShowProjectPanel = true;
    bool m_ShowConsole = true;
    bool m_ShowPerformance = false;

    Entity* m_SelectedEntity = nullptr;
    Entity* m_LookedAtEntity = nullptr;
    Window* m_Window = nullptr;

    std::vector<std::string> m_ConsoleLogs;
};

} // namespace Archura
