#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <vector>

template <typename T> struct ThreadLocal {
  std::function<std::unique_ptr<T>()> getter; // 0
  std::vector<std::unique_ptr<T>> list;       // 32
  std::mutex mtx;                             // 56
  bool success;                               // 96
  ThreadLocal(std::function<std::unique_ptr<T>()> const &);
  void _destory();
  T *_get() const;
  void _init();
  void _set(T *);
  bool checkLocal();
  T &getLocal();
  ~ThreadLocal();
};

static_assert(offsetof(ThreadLocal<void *>, success) == 96);