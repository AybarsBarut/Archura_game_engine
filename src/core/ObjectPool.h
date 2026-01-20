#pragma once

#include <vector>
#include <functional>
#include <memory>

namespace Archura {

/**
 * @brief Object Pool - Sık oluşturulan/silinen nesneleri yönet
 * 
 * Bellek fragmentation ve allocation overhead'ini azaltır.
 * Reusable objects (Projectiles, Particles, vb.) için ideal.
 */
template<typename T>
class ObjectPool {
public:
    explicit ObjectPool(size_t initialCapacity = 100)
        : m_Capacity(initialCapacity) {
        m_Available.reserve(initialCapacity);
        for (size_t i = 0; i < initialCapacity; ++i) {
            m_Available.push_back(std::make_unique<T>());
        }
    }

    /**
     * @brief Pool'dan bir nesne al
     */
    T* Acquire() {
        if (m_Available.empty()) {
            // Capacity'yi 2 katına çıkart
            m_Capacity *= 2;
            for (size_t i = 0; i < m_Capacity / 2; ++i) {
                m_Available.push_back(std::make_unique<T>());
            }
        }

        auto obj = std::move(m_Available.back());
        m_Available.pop_back();
        
        T* ptr = obj.get();
        m_Active[ptr] = std::move(obj);
        return ptr;
    }

    /**
     * @brief Nesneyi pool'a geri koy
     */
    void Release(T* obj) {
        auto it = m_Active.find(obj);
        if (it != m_Active.end()) {
            m_Available.push_back(std::move(it->second));
            m_Active.erase(it);
        }
    }

    /**
     * @brief Aktif nesne sayısını getir
     */
    size_t GetActiveCount() const { return m_Active.size(); }

    /**
     * @brief Kullanılabilir nesne sayısını getir
     */
    size_t GetAvailableCount() const { return m_Available.size(); }

    /**
     * @brief Tüm nesneleri temizle
     */
    void Clear() {
        m_Active.clear();
        m_Available.clear();
    }

private:
    std::vector<std::unique_ptr<T>> m_Available;  // Kullanılabilir nesneler
    std::unordered_map<T*, std::unique_ptr<T>> m_Active;  // Aktif nesneler
    size_t m_Capacity;
};

} // namespace Archura
