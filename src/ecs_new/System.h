#pragma once

#include "Types.h"
#include <set>

namespace Archura::ECS {

class System {
public:
    std::set<Entity> m_Entities;
};

} // namespace Archura::ECS
