#pragma once

#include <string>
#include <vector>
#include "../ecs/Entity.h"

namespace Archura {

    struct ProjectConfig {
        std::string name;
        std::string version;
        std::string startScene;
    };

    class ProjectSerializer {
    public:
        static bool SaveProject(const std::string& path, const ProjectConfig& config, Scene* scene);
        static bool LoadProject(const std::string& path, ProjectConfig& outConfig, Scene* scene);

    private:
        // Helper to serialize an entity to a JSON-like string
        static std::string SerializeEntity(Entity* entity);
    };

} // namespace Archura
