#pragma once

#include "PointerOperations.hpp"

class ControlBlock {
private:
    int refCount = 1;

protected:
    ControlBlock() = default;

public:
    ControlBlock(const ControlBlock&) = delete;

    virtual ~ControlBlock() = default;

    ControlBlock& operator=(const ControlBlock&) = delete;

    int use_count() const noexcept {
        return refCount;
    }

    void addReference() noexcept {
        ++refCount;
    }

    int removeReference() noexcept {
        return --refCount;
    }
};

template <typename T, typename Deleter>
class PointerControlBlock final : public ControlBlock {
private:
    T* ptr;
    Deleter deleter;

public:
    PointerControlBlock(T* pointer, Deleter deletion)
        : ptr(pointer), deleter(Move(deletion)) {}

    ~PointerControlBlock() override {
        deleter(ptr);
    }
};
