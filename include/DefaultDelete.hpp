#pragma once

template <typename T>
class DefaultDelete {
public:
    DefaultDelete() = default;

    template <typename U>
    DefaultDelete(const DefaultDelete<U>&) noexcept {}

    void operator()(T* pointer) const noexcept {
        delete pointer;
    }
};

template <typename T>
class DefaultDelete<T[]> {
public:
    void operator()(T* pointer) const noexcept {
        delete[] pointer;
    }

    template <typename U>
    void operator()(U*) const = delete;
};
