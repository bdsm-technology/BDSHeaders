#pragma once

#include <array>

template <typename T> class buffer_span {
  T const *_start;
  T const *_end;

public:
  class iterator {
    T const *ptr;

  public:
    iterator(T const *);
    bool operator!=(iterator const &rhs) { return ptr != rhs.ptr; }
    T const operator*() { return *ptr; }
    iterator const &operator++() {
      ptr++;
      return *this;
    }
  };
  buffer_span(T const *start, T const *end)
      : _start(start)
      , _end(end) {}
  template <size_t S>
  buffer_span(std::array<T, S> const &arr)
      : buffer_span(arr.data(), arr.data() + sizeof(T)) {}
  iterator begin() const { return iterator(_start); }
  iterator end() const { return iterator(_end); }
};