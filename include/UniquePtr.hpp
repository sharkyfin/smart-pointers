#pragma once

#include "DefaultDelete.hpp"
#include "PointerOperations.hpp"

template <typename T, typename Deleter = DefaultDelete<T>>
class UniquePtr {
private:
    T* ptr = nullptr;
    Deleter deleter{};

public:
    explicit UniquePtr(T* pointer = nullptr) noexcept : ptr(pointer) {}

    UniquePtr(T* pointer, Deleter deletion) noexcept
        : ptr(pointer), deleter(Move(deletion)) {}

    UniquePtr(const UniquePtr&) = delete;

    UniquePtr(UniquePtr&& other) noexcept
        : ptr(other.release()), deleter(Move(other.deleter)) {}

    template <typename U, typename OtherDeleter>
    UniquePtr(UniquePtr<U, OtherDeleter>&& other) noexcept
        : ptr(other.release()), deleter(Move(other.get_deleter())) {}

    template <typename U, typename OtherDeleter>
    UniquePtr(UniquePtr<U[], OtherDeleter>&&) = delete;

    ~UniquePtr() {
        if (ptr) {
            deleter(ptr);
        }
    }

    UniquePtr& operator=(const UniquePtr&) = delete;

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            reset(other.release());
            deleter = Move(other.deleter);
        }
        return *this;
    }

    T* get() const noexcept { return ptr; }

    Deleter& get_deleter() noexcept { return deleter; }

    const Deleter& get_deleter() const noexcept { return deleter; }

    explicit operator bool() const noexcept { return ptr != nullptr; }

    T& operator*() const { return *ptr; }

    T* operator->() const noexcept { return ptr; }

    T* release() noexcept {
        T* old = ptr;
        ptr = nullptr;
        return old;
    }

    void reset(T* newPtr = nullptr) noexcept {
        if (ptr != newPtr) {
            T* old = ptr;
            ptr = newPtr;
            if (old) {
                deleter(old);
            }
        }
    }

    void swap(UniquePtr& other) noexcept {
        Swap(ptr, other.ptr);
        Swap(deleter, other.deleter);
    }
};

template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
private:
    T* ptr = nullptr;
    Deleter deleter{};

public:
    explicit UniquePtr(T* pointer = nullptr) noexcept : ptr(pointer) {}

    UniquePtr(T* pointer, Deleter deletion) noexcept
        : ptr(pointer), deleter(Move(deletion)) {}

    template <typename U>
    UniquePtr(U*) = delete;

    template <typename U>
    UniquePtr(U*, Deleter) = delete;

    UniquePtr(const UniquePtr&) = delete;

    UniquePtr(UniquePtr&& other) noexcept
        : ptr(other.release()), deleter(Move(other.deleter)) {}

    ~UniquePtr() {
        if (ptr) {
            deleter(ptr);
        }
    }

    UniquePtr& operator=(const UniquePtr&) = delete;

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            reset(other.release());
            deleter = Move(other.deleter);
        }
        return *this;
    }

    T* get() const noexcept { return ptr; }

    Deleter& get_deleter() noexcept { return deleter; }

    const Deleter& get_deleter() const noexcept { return deleter; }

    explicit operator bool() const noexcept { return ptr != nullptr; }

    T& operator[](int index) const { return ptr[index]; }

    T* release() noexcept {
        T* old = ptr;
        ptr = nullptr;
        return old;
    }

    void reset(T* newPtr = nullptr) noexcept {
        if (ptr != newPtr) {
            T* old = ptr;
            ptr = newPtr;
            if (old) {
                deleter(old);
            }
        }
    }

    template <typename U>
    void reset(U*) = delete;

    void swap(UniquePtr& other) noexcept {
        Swap(ptr, other.ptr);
        Swap(deleter, other.deleter);
    }
};
