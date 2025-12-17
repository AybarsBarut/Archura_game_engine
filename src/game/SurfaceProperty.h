#pragma once

#include "../ecs/Component.h"

namespace Archura {

enum class SurfaceType {
    Concrete,
    Wood,
    Metal,
    Flesh,
    Dirt,
    Glass
};

/**
 * @brief Surface Property component - Defines physical material properties
 */
struct SurfaceProperty : public Component {
    SurfaceType type = SurfaceType::Concrete;
    
    SurfaceProperty(SurfaceType t = SurfaceType::Concrete) : type(t) {}
};

} // namespace Archura
