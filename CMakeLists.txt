cmake_minimum_required(VERSION 3.16)
project(SmartPointers LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(smartpointers INTERFACE)
target_include_directories(smartpointers INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_features(smartpointers INTERFACE cxx_std_17)

target_compile_options(smartpointers INTERFACE
    "$<$<CXX_COMPILER_ID:MSVC>:/W4;/WX>"
    "$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall;-Wextra;-Wpedantic;-Werror>"
)

option(ENABLE_SANITIZERS "Enable ASan and UBSan" OFF)
if(ENABLE_SANITIZERS)
    target_compile_options(smartpointers INTERFACE
        -fsanitize=address,undefined
        -fno-sanitize-recover=all
        -fno-omit-frame-pointer
    )
    target_link_options(smartpointers INTERFACE -fsanitize=address,undefined)
endif()

add_executable(smart_ptr_demo main.cpp)
target_link_libraries(smart_ptr_demo PRIVATE smartpointers)

include(CTest)
if(BUILD_TESTING)
    include(FetchContent)
    FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.17.0
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(googletest)

    add_executable(smart_ptr_tests tests/pointer_tests.cpp)
    target_link_libraries(smart_ptr_tests PRIVATE smartpointers GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(smart_ptr_tests)
endif()
