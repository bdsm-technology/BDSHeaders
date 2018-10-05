#pragma once

#include <atomic>

namespace Lockless {

template <typename T> struct WeakAtomic {
  std::atomic<T> data;
  WeakAtomic();
  template <typename U> WeakAtomic(U &&);
  T load();
  operator T();
  template <typename U> T &operator=(U &&);
};

} // namespace Lockless