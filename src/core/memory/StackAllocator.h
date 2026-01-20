#pragma once

#include "Allocator.h"
#include <vector>

namespace Archura {

class StackAllocator : public Allocator {
public:
    StackAllocator(size_t sizeBytes);
    ~StackAllocator() override;

    void* Allocate(size_t size, size_t alignment = 8) override;
    void Free(void* ptr) override; // Stack allocator frees in LIFO order (marker based)
    void Reset() override;

    // Additional Stack specific methods
    using Marker = size_t;
    Marker GetMarker() const;
    void FreeToMarker(Marker marker);

private:
    void* m_Start = nullptr;
    size_t m_Size = 0;
    size_t m_Offset = 0; // Current top of stack
};

} // namespace Archura
