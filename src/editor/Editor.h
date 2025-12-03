#pragma once

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <streambuf>
#include <filesystem>
#include <glm/glm.hpp>

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
    // Looked at entity (Debug)
    void SetLookedAtEntity(Entity* entity) { m_LookedAtEntity = entity; }
    
    // Spawn Position (from Raycast)
    void SetSpawnPosition(const glm::vec3& pos) { m_SpawnPosition = pos; }

private:
    void SpawnEntity(Scene* scene, const std::string& type);

    void SetupLayout();
    void DrawMenuBar(Scene* scene);
    void DrawToolbar();
    void DrawSceneHierarchy(Scene* scene);
    void DrawInspector();
    void DrawProjectPanel();
    void DrawConsolePanel();
    void DrawPerformanceMetrics(float deltaTime, float fps);
    void DrawDemoWindow();
    
    void ExecuteCommand(const char* command);

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
    glm::vec3 m_SpawnPosition = glm::vec3(0.0f);
    Window* m_Window = nullptr;

    std::vector<std::string> m_ConsoleLogs;
    char m_InputBuf[256] = "";
    
    // Project Browser
    std::filesystem::path m_BaseProjectDir;
    std::filesystem::path m_CurrentProjectDir;

    // Stream redirection
    std::streambuf* m_OldCoutBuf = nullptr;
    std::unique_ptr<std::streambuf> m_NewCoutBuf;
};

// Custom streambuf to redirect cout to Editor
class EditorStreamBuf : public std::streambuf {
public:
    EditorStreamBuf(Editor* editor) : m_Editor(editor) {}

protected:
    virtual int_type overflow(int_type c) override {
        if (c != EOF) {
            char ch = static_cast<char>(c);
            if (ch == '\n') {
                m_Editor->Log(m_Buffer);
                m_Buffer.clear();
            } else {
                m_Buffer += ch;
            }
        }
        return c;
    }

private:
    Editor* m_Editor;
    std::string m_Buffer;
};

} // namespace Archura
