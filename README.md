# Учебные умные указатели (C++17)

`UniquePtr` и `SharedPtr`. Тесты — GoogleTest, замеры времени — в `main.cpp`.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DENABLE_SANITIZERS=OFF
cmake --build build
ctest --test-dir build --output-on-failure
./build/smart_ptr_demo
```

Для санитайзеров при настройке указать `-DCMAKE_BUILD_TYPE=Debug -DENABLE_SANITIZERS=ON`.

Отличия от `std`: счётчики — `int`, нет потокобезопасности, `weak_ptr` и `make_shared`.
