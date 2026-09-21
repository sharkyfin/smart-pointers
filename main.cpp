#include "SharedPtr.hpp"
#include "UniquePtr.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

double measureRaw(int count, long long &sum) {
  auto start = std::chrono::steady_clock::now();
  {
    std::vector<int *> pointers;
    pointers.reserve(count);

    for (int i = 0; i < count; ++i) {
      pointers.push_back(new int(i));
    }

    for (int *pointer : pointers) {
      sum += *pointer;
    }

    for (int *pointer : pointers) {
      delete pointer;
    }
  }

  return std::chrono::duration<double, std::milli>(
             std::chrono::steady_clock::now() - start).count();
}

template <typename Pointer>
double measurePointers(int count, long long &sum) {
  auto start = std::chrono::steady_clock::now();
  {
    std::vector<Pointer> pointers;
    pointers.reserve(count);

    for (int i = 0; i < count; ++i) {
      pointers.emplace_back(new int(i));
    }

    for (const Pointer &pointer : pointers) {
      sum += *pointer;
    }
  }

  return std::chrono::duration<double, std::milli>(
             std::chrono::steady_clock::now() - start).count();
}

int main() {
  int sizes[] = {1000, 10000, 100000, 1000000};

  std::cout << std::left << std::setw(10) << "N" << std::right << std::setw(12)
            << "raw_ms" << std::setw(16) << "UniquePtr_ms" << std::setw(16)
            << "std_unique_ms" << std::setw(16) << "SharedPtr_ms"
            << std::setw(16) << "std_shared_ms" << '\n';

  for (int count : sizes) {
    long long rawSum = 0;
    long long uniqueSum = 0;
    long long stdUniqueSum = 0;
    long long sharedSum = 0;
    long long stdSharedSum = 0;

    double rawTime = measureRaw(count, rawSum);
    double uniqueTime = measurePointers<UniquePtr<int>>(count, uniqueSum);
    double stdUniqueTime =
        measurePointers<std::unique_ptr<int>>(count, stdUniqueSum);
    double sharedTime = measurePointers<SharedPtr<int>>(count, sharedSum);
    double stdSharedTime =
        measurePointers<std::shared_ptr<int>>(count, stdSharedSum);

    if (rawSum != uniqueSum || rawSum != stdUniqueSum || rawSum != sharedSum ||
        rawSum != stdSharedSum) {
      return 1;
    }

    std::cout << std::left << std::setw(10) << count << std::right << std::fixed
              << std::setprecision(3) << std::setw(12) << rawTime
              << std::setw(16) << uniqueTime << std::setw(16) << stdUniqueTime
              << std::setw(16) << sharedTime << std::setw(16) << stdSharedTime
              << '\n';
  }

  return 0;
}
