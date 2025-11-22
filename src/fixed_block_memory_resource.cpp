#include "../include/fixed_block_memory_resource.h"
#include <iostream>
#include <algorithm>
#include <cstdint>

FixedBlockMemoryResource::FixedBlockMemoryResource(size_t size) 
    : m_buffer_size(size), m_owns_memory(true) {
    m_buffer_start = ::operator new(size);
}

FixedBlockMemoryResource::~FixedBlockMemoryResource() {
    if (m_owns_memory && m_buffer_start) {
        ::operator delete(m_buffer_start);
    }
}

void* FixedBlockMemoryResource::do_allocate(size_t bytes, size_t alignment) {
    // Сортируем блоки по адресам для поиска свободных промежутков
    m_allocated_blocks.sort();

    void* current_pos = m_buffer_start;

    // Функция для выравнивания указателя на границу alignment
    // Формула: (ptr + align - 1) & ~(align - 1)
    auto align_forward = [](void* ptr, size_t align) -> void* {
        std::uintptr_t uptr = reinterpret_cast<std::uintptr_t>(ptr);
        std::uintptr_t aligned = (uptr + align - 1) & ~(align - 1);
        return reinterpret_cast<void*>(aligned);
    };

    // Ищем свободное место между уже выделенными блоками
    // Алгоритм: проходим по отсортированным блокам и ищем промежуток
    for (const auto& block : m_allocated_blocks) {
        void* potential_ptr = align_forward(current_pos, alignment);
        char* potential_end = static_cast<char*>(potential_ptr) + bytes;
        char* next_block_start = static_cast<char*>(block.ptr);

        // Если между current_pos и block.ptr помещается запрошенный размер
        if (potential_end <= next_block_start) {
            m_allocated_blocks.push_back({potential_ptr, bytes});
            return potential_ptr;
        }

        // Переходим к концу текущего блока и ищем дальше
        current_pos = static_cast<char*>(block.ptr) + block.size;
    }

    // Если не нашли место между блоками, пробуем в конце буфера
    void* potential_ptr = align_forward(current_pos, alignment);
    char* buffer_end = static_cast<char*>(m_buffer_start) + m_buffer_size;

    if (static_cast<char*>(potential_ptr) + bytes <= buffer_end) {
        m_allocated_blocks.push_back({potential_ptr, bytes});
        return potential_ptr;
    }

    throw std::bad_alloc();
}

void FixedBlockMemoryResource::do_deallocate(void* p, size_t bytes, size_t alignment) {
    // Ищем блок в списке по указателю
    // После удаления из списка память становится доступной для переиспользования
    auto it = std::find_if(m_allocated_blocks.begin(), m_allocated_blocks.end(),
                           [p](const BlockInfo& b) { return b.ptr == p; });
    
    if (it != m_allocated_blocks.end()) {
        m_allocated_blocks.erase(it);
        // Память физически не очищается, но место становится свободным
        // и будет переиспользовано при следующем do_allocate
    }
}

bool FixedBlockMemoryResource::do_is_equal(const std::pmr::memory_resource& other) const noexcept {
    return this == &other;
}