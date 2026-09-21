#pragma once

template <typename T>
T&& Move(T& value) noexcept {
    return static_cast<T&&>(value);
}

template <typename T>
void Swap(T& first, T& second) noexcept {
    T temporary = Move(first);
    first = Move(second);
    second = Move(temporary);
}
