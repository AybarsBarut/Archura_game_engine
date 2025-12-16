#pragma once

#include <cstddef>
#include <cstdint>

namespace Archura {

class Allocator {
public:
    virtual ~Allocator() = default;

    virtual void* Allocate(size_t size, size_t alignment = 8) = 0;
    virtual void Free(void* ptr) = 0;
    virtual void Reset() = 0;
};

} // namespace Archura
