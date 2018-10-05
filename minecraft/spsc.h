#pragma once

#include "Lockless.h"

template <typename T, std::size_t size = 512ul> struct SPSCQueue {
  struct Block {
    char filler[160];
    Block(uint64_t const &, char *, char *);
  };
  enum class AllocationMode {};
  Lockless::WeakAtomic<Block *> head; // 0
  char filler8[56];
  Lockless::WeakAtomic<Block *> tail; // 64
  std::size_t number;                 // 72
  template <typename U> static std::size_t align_for(char *);
  static void *make_block(std::size_t number);
  static std::size_t ceilToPow2(std::size_t number);
  template <typename U> bool enqueue(U &&);
  bool enqueue_copy(T const &);
  template <AllocationMode mode, typename U> bool inner_enqueue(U &&);
  template <typename U> bool try_dequeue(U &);
  std::size_t size_approx() const;
  SPSCQueue(uint64_t);
  ~SPSCQueue();
};

static_assert(sizeof(SPSCQueue<int, 512>) == 80);