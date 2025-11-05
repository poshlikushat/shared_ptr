#pragma once

#include <cstddef>
#include <stdexcept>
#include <tuple>
#include <memory>

template<typename T, typename D = std::default_delete<T>>
class SharedPtr {
    struct ControlBlock {
        std::tuple<T*, size_t, D> data;
    };

    ControlBlock* cb = nullptr;

    void increment_ref_count() const {
        if (cb) ++std::get<1>(cb->data);
    }

    void decrement_ref_count() {
        if (cb) {
            auto& cnt = std::get<1>(cb->data);
            if (--cnt == 0) {
                auto& del = std::get<2>(cb->data);
                del(std::get<0>(cb->data));
                delete cb;
            }
        }
    }

public:
    explicit SharedPtr(T* ptr = nullptr)
        : cb(ptr ? new ControlBlock{std::make_tuple(ptr, size_t{1}, D{})} : nullptr) {}

    SharedPtr(const SharedPtr& other) : cb(other.cb) {
        increment_ref_count();
    }

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

    SharedPtr& operator=(SharedPtr&& other) noexcept {
        if (this != &other) {
            decrement_ref_count();
            cb = std::exchange(other.cb, nullptr);
        }
        return *this;
    }

    void reset(T* other = nullptr) {
        decrement_ref_count();
        cb = other ? new ControlBlock{std::make_tuple(other, size_t{1}, D{})} : nullptr;
    }

    [[nodiscard]] size_t get_count() const {
        return cb ? std::get<1>(cb->data) : 0;
    }

    void swap(SharedPtr& other) noexcept {
        std::swap(cb, other.cb);
    }

    T* operator->() const {
        if (!cb || !std::get<0>(cb->data)) throw std::runtime_error("Pointer is null");
        return std::get<0>(cb->data);
    }

    T& operator*() const {
        if (!cb || !std::get<0>(cb->data)) throw std::runtime_error("Pointer is null");
        return *std::get<0>(cb->data);
    }

    T* get() const {
        return cb ? std::get<0>(cb->data) : nullptr;
    }

    ~SharedPtr() {
        decrement_ref_count();
    }
};

template<typename T, typename... Args>
SharedPtr<T> make_shared(Args&&... args) {
    return SharedPtr<T>(new T(std::forward<Args>(args)...));
}