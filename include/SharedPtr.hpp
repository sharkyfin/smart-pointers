#pragma once

#include "ControlBlock.hpp"
#include "DefaultDelete.hpp"

template <typename T>
class SharedPtr {
private:
    T* ptr = nullptr;
    ControlBlock* control = nullptr;

    template <typename U>
    friend class SharedPtr;

public:
    explicit SharedPtr(T* pointer = nullptr) {
        reset(pointer);
    }

    template <typename U>
    explicit SharedPtr(U* pointer) : SharedPtr(pointer, DefaultDelete<U>()) {}

    template <typename U, typename Deleter>
    SharedPtr(U* pointer, Deleter deletion) : ptr(pointer) {
        if (!pointer) {
            return;
        }
        try {
            control = new PointerControlBlock<U, Deleter>(pointer, deletion);
        } catch (...) {
            deletion(pointer);
            throw;
        }
    }

    SharedPtr(const SharedPtr& other) noexcept : ptr(other.ptr), control(other.control) {
        if (control) {
            control->addReference();
        }
    }

    template <typename U>
    SharedPtr(const SharedPtr<U>& other) noexcept : ptr(other.ptr), control(other.control) {
        if (control) {
            control->addReference();
        }
    }

    template <typename U>
    SharedPtr(const SharedPtr<U[]>&) = delete;

    SharedPtr(SharedPtr&& other) noexcept {
        swap(other);
    }

    template <typename U>
    SharedPtr(SharedPtr<U>&& other) noexcept
        : ptr(other.ptr), control(other.control) {
        other.ptr = nullptr;
        other.control = nullptr;
    }

    template <typename U>
    SharedPtr(SharedPtr<U[]>&&) = delete;

    ~SharedPtr() {
        if (control && control->removeReference() == 0) {
            delete control;
        }
    }

    SharedPtr& operator=(SharedPtr other) noexcept {
        swap(other);
        return *this;
    }

    T* get() const noexcept { return ptr; }

    int use_count() const noexcept { return control ? control->use_count() : 0; }

    explicit operator bool() const noexcept { return ptr != nullptr; }

    T& operator*() const { return *ptr; }

    T* operator->() const noexcept { return ptr; }

    void reset() noexcept {
        SharedPtr empty;
        swap(empty);
    }

    template <typename U>
    void reset(U* newPtr) {
        reset(newPtr, DefaultDelete<U>());
    }

    template <typename U, typename Deleter>
    void reset(U* newPtr, Deleter deletion) {
        if (ptr != newPtr) {
            SharedPtr replacement(newPtr, Move(deletion));
            swap(replacement);
        }
    }

    void swap(SharedPtr& other) noexcept {
        Swap(ptr, other.ptr);
        Swap(control, other.control);
    }
};

template <typename T>
class SharedPtr<T[]> {
private:
    T* ptr = nullptr;
    ControlBlock* control = nullptr;

public:
    explicit SharedPtr(T* pointer = nullptr) {
        reset(pointer);
    }

    template <typename Deleter>
    SharedPtr(T* pointer, Deleter deletion) : ptr(pointer) {
        if (!pointer) {
            return;
        }
        try {
            control = new PointerControlBlock<T, Deleter>(pointer, deletion);
        } catch (...) {
            deletion(pointer);
            throw;
        }
    }

    template <typename U>
    SharedPtr(U*) = delete;

    template <typename U, typename Deleter>
    SharedPtr(U*, Deleter) = delete;

    SharedPtr(const SharedPtr& other) noexcept : ptr(other.ptr), control(other.control) {
        if (control) {
            control->addReference();
        }
    }

    SharedPtr(SharedPtr&& other) noexcept {
        swap(other);
    }

    ~SharedPtr() {
        if (control && control->removeReference() == 0) {
            delete control;
        }
    }

    SharedPtr& operator=(SharedPtr other) noexcept {
        swap(other);
        return *this;
    }

    T* get() const noexcept { return ptr; }

    int use_count() const noexcept { return control ? control->use_count() : 0; }

    explicit operator bool() const noexcept { return ptr != nullptr; }

    T& operator[](int index) const { return ptr[index]; }

    void reset() noexcept {
        SharedPtr empty;
        swap(empty);
    }

    void reset(T* newPtr) {
        reset(newPtr, DefaultDelete<T[]>());
    }

    template <typename Deleter>
    void reset(T* newPtr, Deleter deletion) {
        if (ptr != newPtr) {
            SharedPtr replacement(newPtr, Move(deletion));
            swap(replacement);
        }
    }

    template <typename U>
    void reset(U*) = delete;

    template <typename U, typename Deleter>
    void reset(U*, Deleter) = delete;

    void swap(SharedPtr& other) noexcept {
        Swap(ptr, other.ptr);
        Swap(control, other.control);
    }
};
