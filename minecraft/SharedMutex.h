#pragma once

#include <shared_mutex>

struct SharedMutex {
  std::shared_timed_mutex mtx;
  void lock();
  bool try_lock();
  bool try_lock_shared();
  void unlock();
  void unlock_shared();
};