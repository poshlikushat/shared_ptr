#pragma once
#include <cstddef>
#include <utility>
#include <stdexcept>

template<typename T> class SharedPtr {
    struct ControlBlock {
        T* ptr;
        size_t ref_count;
    };
    ControlBlock* cb;
    void increment_ref_count() const {
        if (cb) ++(cb->ref_count);
    }
    
    void decrement_ref_count() {
        if (cb) {
            if (--cb->ref_count == 0) {
                delete cb->ptr;
                delete cb;
            }
            cb = nullptr;
        }
    }
    
    public:
        explicit SharedPtr(T* ptr = nullptr) : cb(ptr ? new ControlBlock{ptr, 1} : nullptr) {};
        
        SharedPtr<T>(const SharedPtr<T>& other) :cb(other.cb) {
            increment_ref_count();
        };
        
        SharedPtr& operator=(const SharedPtr& other) {
        if (this != &other) {
            decrement_ref_count();
            cb = other.cb;
            increment_ref_count();
        }
        return *this;
    }
        SharedPtr(SharedPtr&& other) noexcept
            : cb(std::exchange(other.cb, nullptr)) {}

        
        SharedPtr<T>& operator=(SharedPtr<T>&& other) noexcept {
            if (this != &other) {
                decrement_ref_count();
                cb = std::exchange(other.cb, nullptr);
            }
            return *this;
        }

        
        void reset(T* other = nullptr) {
            decrement_ref_count();
            cb = other ? new ControlBlock{other, 1} : nullptr;
        }
        
        [[nodiscard]] size_t get_count() const {
            return cb ? cb->ref_count : 0;
        }
        
        void swap(SharedPtr& other) noexcept {
                std::swap(cb, other.cb);
            }
        
        T* operator->() const {
            if (!cb || !cb->ptr) throw std::runtime_error("Pointer is null");
            return cb->ptr;
        }
        
        T& operator*() const {
            if (!cb || !cb->ptr) throw std::runtime_error("Pointer is null");
            return *cb->ptr;
        }
        
        T* get() const {
            return cb ? cb->ptr : nullptr;
        }
        
        ~SharedPtr() {
            decrement_ref_count();
        }
};

template<typename T, typename ... Args>
    SharedPtr<T> make_shared(Args&&... args) {
    return SharedPtr<T>(new T(std::forward<Args>(args)...));
}

