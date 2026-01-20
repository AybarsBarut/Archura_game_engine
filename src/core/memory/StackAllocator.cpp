#include "StackAllocator.h"
#include <cstdlib>
#include <cassert>
#include <iostream>

namespace Archura {

StackAllocator::StackAllocator(size_t sizeBytes) 
    : m_Size(sizeBytes)
    , m_Offset(0) 
{
    m_Start = std::malloc(m_Size);
#ifdef _DEBUG
    std::cout << "[StackAllocator] Initialized with " << sizeBytes << " bytes.\n";
#endif
}

StackAllocator::~StackAllocator() {
    std::free(m_Start);
}

void* StackAllocator::Allocate(size_t size, size_t alignment) {
    // Current address
    uintptr_t currentAddress = (uintptr_t)m_Start + m_Offset;

    // Calculate padding for alignment
    size_t padding = 0;
    if (alignment != 0) {
        size_t multiplier = (currentAddress / alignment) + 1;
        uintptr_t alignedAddress = multiplier * alignment;
        padding = alignedAddress - currentAddress;
    }

    if (m_Offset + padding + size > m_Size) {
#ifdef _DEBUG
        std::cerr << "[StackAllocator] Out of Memory!\n";
#endif
        return nullptr;
    }

    m_Offset += padding;
    uintptr_t nextAddress = currentAddress + padding;
    m_Offset += size;

    // We don't store headers here to keep it raw and fast.
    // User must handle FreeToMarker correctly.
    
    return (void*)nextAddress;
}

void StackAllocator::Free(void* ptr) {
    // Stack Allocator doesn't support individual Free.
    // Use FreeToMarker or Reset.
}

void StackAllocator::Reset() {
    m_Offset = 0;
}

StackAllocator::Marker StackAllocator::GetMarker() const {
    return m_Offset;
}

void StackAllocator::FreeToMarker(Marker marker) {
    m_Offset = marker;
}

} // namespace Archura
