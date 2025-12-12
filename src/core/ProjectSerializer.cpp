#include "ProjectSerializer.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>

namespace Archura {

    bool ProjectSerializer::SaveProject(const std::string& path, const ProjectConfig& config, Scene* scene) {
        std::filesystem::path projectPath(path);
        if (!std::filesystem::exists(projectPath.parent_path())) {
            std::filesystem::create_directories(projectPath.parent_path());
        }

        std::ofstream file(path);
        if (!file.is_open()) {
            std::cerr << "Failed to save project file: " << path << std::endl;
            return false;
        }

        // Simple JSON construction
        file << "{\n";
        file << "  \"project\": {\n";
        file << "    \"name\": \"" << config.name << "\",\n";
        file << "    \"version\": \"" << config.version << "\",\n";
        file << "    \"start_scene\": \"" << config.startScene << "\"\n";
        file << "  },\n";
        file << "  \"entities\": [\n";

        auto& entities = scene->GetEntities();
        for (size_t i = 0; i < entities.size(); ++i) {
            file << SerializeEntity(entities[i].get());
            if (i < entities.size() - 1) {
                file << ",";
            }
            file << "\n";
        }

        file << "  ]\n";
        file << "}\n";

        file.close();

        return true;
    }

    bool ProjectSerializer::LoadProject(const std::string& path, ProjectConfig& outConfig, Scene* scene) {
        // Basic parsing logic would go here. 
        // For this implementation, we will focus on the structure.
        // In a real scenario, use a library like nlohmann/json.

        return true;
    }

    std::string ProjectSerializer::SerializeEntity(Entity* entity) {
        std::stringstream ss;
        ss << "    {\n";
        ss << "      \"id\": " << entity->GetID() << ",\n";
        ss << "      \"name\": \"" << entity->GetName() << "\",\n";
        
        auto* transform = entity->GetComponent<Transform>();
        if (transform) {
            ss << "      \"transform\": {\n";
            ss << "        \"position\": [" << transform->position.x << ", " << transform->position.y << ", " << transform->position.z << "],\n";
            ss << "        \"rotation\": [" << transform->rotation.x << ", " << transform->rotation.y << ", " << transform->rotation.z << "],\n";
            ss << "        \"scale\": [" << transform->scale.x << ", " << transform->scale.y << ", " << transform->scale.z << "]\n";
            ss << "      }\n";
        }
        
        ss << "    }";
        return ss.str();
    }

} // namespace Archura
