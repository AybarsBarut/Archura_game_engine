#include "PoolAllocator.h"
#include <cstdlib>
#include <cassert>
#include <algorithm> // for std::max
#include <iostream>

namespace Archura {

PoolAllocator::PoolAllocator(size_t objectSize, size_t objectAlignment, size_t sizeBytes)
    : m_ObjectSize(objectSize)
    , m_ObjectAlignment(objectAlignment)
    , m_Size(sizeBytes)
{
    // Ensure object size is large enough to hold the pointer for free list
    m_ObjectSize = std::max(objectSize, sizeof(FreeHeader*));
    
    // Allocate simple block
    m_Start = std::malloc(m_Size);
    Reset();
    
#ifdef _DEBUG
    std::cout << "[PoolAllocator] Initialized. Block Size: " << m_ObjectSize << ", Capacity: " << (m_Size / m_ObjectSize) << "\n";
#endif
}

PoolAllocator::~PoolAllocator() {
    std::free(m_Start);
}

void PoolAllocator::Reset() {
    m_UsedMemory = 0;
    m_NumAllocations = 0;
    
    // Calculate actual number of chunks
    size_t adjustment = 0;
    uintptr_t currentAddress = (uintptr_t)m_Start;
    
    // Align first block
    size_t multiplier = (currentAddress / m_ObjectAlignment) + 1;
    // Align first block if necessary (basic implementation assumes Start is aligned)
    size_t numObjects = m_Size / m_ObjectSize;
    
    m_FreeList = nullptr;
    
    // Initialize free list
    // Initialize free list
    
    uintptr_t p = (uintptr_t)m_Start;
    for (size_t i = 0; i < numObjects; ++i) {
        FreeHeader* header = (FreeHeader*)p;
        header->next = m_FreeList;
        m_FreeList = header;
        p += m_ObjectSize;
    }
}

void* PoolAllocator::Allocate(size_t size, size_t alignment) {
    // Pool allocator expects fixed size allocations.
    
    if (m_FreeList == nullptr) {
#ifdef _DEBUG
        std::cerr << "[PoolAllocator] Out of Memory!\n";
#endif
        return nullptr;
    }
    
    void* p = m_FreeList;
    m_FreeList = m_FreeList->next;
    
    m_UsedMemory += m_ObjectSize;
    m_NumAllocations++;
    
    return p;
}

void PoolAllocator::Free(void* ptr) {
    if (ptr == nullptr) return;
    
    // Push back to free list
    FreeHeader* header = (FreeHeader*)ptr;
    header->next = m_FreeList;
    m_FreeList = header;
    
    m_UsedMemory -= m_ObjectSize;
    m_NumAllocations--;
}

} // namespace Archura
