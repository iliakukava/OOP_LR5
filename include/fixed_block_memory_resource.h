#ifndef FIXED_BLOCK_MEMORY_RESOURCE_H
#define FIXED_BLOCK_MEMORY_RESOURCE_H

#include <memory_resource>
#include <list>
#include <cstddef>

class FixedBlockMemoryResource : public std::pmr::memory_resource {
private:
    struct BlockInfo {
        void* ptr;
        size_t size;
        
        bool operator<(const BlockInfo& other) const {
            return ptr < other.ptr;
        }
    };

    void* m_buffer_start;
    size_t m_buffer_size;
    std::list<BlockInfo> m_allocated_blocks;
    bool m_owns_memory;

public:
    explicit FixedBlockMemoryResource(size_t size);
    ~FixedBlockMemoryResource();

protected:
    void* do_allocate(size_t bytes, size_t alignment) override;
    void do_deallocate(void* p, size_t bytes, size_t alignment) override;
    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override;
};

#endif 