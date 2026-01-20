#pragma once

#include "Allocator.h"

namespace Archura {

class PoolAllocator : public Allocator {
public:
    PoolAllocator(size_t objectSize, size_t objectAlignment, size_t sizeBytes);
    ~PoolAllocator() override;

    void* Allocate(size_t size, size_t alignment = 8) override;
    void Free(void* ptr) override;
    void Reset() override;

private:
    struct FreeHeader {
        FreeHeader* next;
    };

    void* m_Start = nullptr;
    size_t m_ObjectSize;
    size_t m_ObjectAlignment;
    size_t m_Size;
    size_t m_UsedMemory = 0;
    size_t m_NumAllocations = 0;

    FreeHeader* m_FreeList = nullptr;
};

} // namespace Archura
