//
// Created by Ian on 21-1-2026.
//

#ifndef PERFNETPROXY_OBJECTPOOL_HPP
#define PERFNETPROXY_OBJECTPOOL_HPP

#include <atomic>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>

template<typename T, std::size_t numOfSlabObjects>
class GlobalObjectPool {
    struct Slab {
        std::array<T, numOfSlabObjects> m_ObjectStorage {};
        std::array<uint16_t, numOfSlabObjects> m_ObjectStorageFreeIndexing {0};
        uint16_t m_FreeCount{numOfSlabObjects};

        Slab() {
            for (uint16_t i = 0; i < numOfSlabObjects; ++i)
                m_ObjectStorageFreeIndexing[i] = i;
        };
    };

public:
    explicit GlobalObjectPool(std::size_t initialCapacity = 32) : m_GlobalSlabs(initialCapacity), m_AvailableSlabs(initialCapacity), m_FreeSlabs(initialCapacity) {

    }

    Slab *acquire() {
        if (m_FreeSlabs.load(std::memory_order::memory_order_consume) == 0) {
            expand();
        }

        std::lock_guard lock(m_InternalMutexLock);

        Slab* slab = m_AvailableSlabs.back();
        m_AvailableSlabs.pop_back();

        m_FreeSlabs.fetch_sub(1, std::memory_order_release);

        return slab;
    }

    void shrink(std::size_t ) {

    }

private:
    std::vector<std::unique_ptr<Slab>> m_GlobalSlabs;
    std::vector<Slab*> m_AvailableSlabs;
    std::atomic<std::uint32_t> m_FreeSlabs{0};
    std::mutex m_InternalMutexLock {};

    void expand(std::size_t grow_size = 32) {

    }


};


class LocalObjectPool {
};
#endif //PERFNETPROXY_OBJECTPOOL_HPP
