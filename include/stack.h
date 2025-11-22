#ifndef STACK_H
#define STACK_H

#include <memory_resource>
#include <iterator>
#include <stdexcept>
#include <cstddef>

template<typename T>
class Stack {
private:
    std::pmr::polymorphic_allocator<T> alloc_;
    T* data_ = nullptr;
    size_t size_ = 0;
    size_t capacity_ = 0;

    void grow() {
        size_t new_cap = capacity_ ? capacity_ * 2 : 4;
        T* new_data = alloc_.allocate(new_cap);
        
        // КОПИРУЕМ в новый массив ПЕРЕД destroy
        for (size_t i = 0; i < size_; ++i) {
            alloc_.construct(&new_data[i], std::move_if_noexcept(data_[i]));
        }
        
        // Destroy старого массива
        for (size_t i = 0; i < size_; ++i) {
            alloc_.destroy(&data_[i]);
        }
        if (data_) {
            alloc_.deallocate(data_, capacity_);
        }
        
        data_ = new_data;
        capacity_ = new_cap;
    }

    void shrink() {
        if (capacity_ > 8 && size_ < capacity_ / 4) {
            size_t new_cap = capacity_ / 2;
            T* new_data = alloc_.allocate(new_cap);
            
            for (size_t i = 0; i < size_; ++i) {
                alloc_.construct(&new_data[i], std::move_if_noexcept(data_[i]));
            }
            
            for (size_t i = 0; i < size_; ++i) {
                alloc_.destroy(&data_[i]);
            }
            alloc_.deallocate(data_, capacity_);
            
            data_ = new_data;
            capacity_ = new_cap;
        }
    }

public:
    using allocator_type = std::pmr::polymorphic_allocator<T>;
    
    explicit Stack(const allocator_type& a) : alloc_(a) {}
    ~Stack() { clear(); }
    
    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;

    void push(const T& value) {
        if (size_ == capacity_) grow();
        alloc_.construct(&data_[size_++], value);
    }
    
    void pop() {
        if (empty()) return;
        alloc_.destroy(&data_[--size_]);
        shrink();
    }
    
    T& top() { 
        if (empty()) throw std::out_of_range("Stack is empty");
        return data_[size_ - 1]; 
    }
    const T& top() const { 
        if (empty()) throw std::out_of_range("Stack is empty");
        return data_[size_ - 1]; 
    }
    
    bool empty() const { return size_ == 0; }
    size_t size() const { return size_; }
    
    void clear() {
        for (size_t i = 0; i < size_; ++i) {
            alloc_.destroy(&data_[i]);
        }
        if (data_) {
            alloc_.deallocate(data_, capacity_);
            data_ = nullptr;
        }
        size_ = 0;
        capacity_ = 0;
    }

    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        Iterator(T* ptr) : ptr_(ptr) {}
        
        reference operator*() const { return *ptr_; }
        pointer operator->() const { return ptr_; }
        
        Iterator& operator++() { 
            --ptr_;  // Идем от top к bottom
            return *this; 
        }
        Iterator operator++(int) { 
            Iterator tmp = *this; 
            ++(*this); 
            return tmp; 
        }
        
        bool operator==(const Iterator& other) const { return ptr_ == other.ptr_; }
        bool operator!=(const Iterator& other) const { return ptr_ != other.ptr_; }
        
    private:
        T* ptr_;
    };

    Iterator begin() { 
        return size_ ? Iterator(data_ + size_ - 1) : Iterator(data_ - 1); 
    }
    Iterator end() { 
        return Iterator(data_ - 1); 
    }
};

#endif
