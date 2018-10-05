#pragma once

#include <dlfcn.h>

template <typename T> static void *void_cast(T f) {
  union {
    T pf;
    void *p;
  };
  pf = f;
  return p;
}

static void *MinecraftHandle() {
  static void *handle = dlopen(nullptr, RTLD_LAZY);
  return handle;
}

extern "C" {

void *MSHookFunction(void *symbol, void *hook, void **original);

#define CONS(a, b) a##b
#define HOOK(type, method) mcpelauncher_hook(void_cast(&type ::method), void_cast(&type ::CONS($$, method)), (void **)&type ::CONS($, method)())
#define IMPL(type, ret, name, suffix, ...)                                                                                                           \
  using CONS(name, _t) = ret (*)(type suffix *, ##__VA_ARGS__);                                                                                      \
  ret name(__VA_ARGS__) suffix;                                                                                                                      \
  static CONS(name, _t) & CONS($, name)() {                                                                                                          \
    static CONS(name, _t) fn;                                                                                                                        \
    return fn;                                                                                                                                       \
  }                                                                                                                                                  \
  ret CONS($$, name)(__VA_ARGS__) suffix
#define IMPL_STATIC(ret, name, ...)                                                                                                                  \
  using CONS(name, _t) = ret (*)(__VA_ARGS__);                                                                                                       \
  static ret name(__VA_ARGS__);                                                                                                                      \
  static CONS(name, _t) & CONS($, name)() {                                                                                                          \
    static CONS(name, _t) fn;                                                                                                                        \
    return fn;                                                                                                                                       \
  }                                                                                                                                                  \
  static ret CONS($$, name)(__VA_ARGS__)
}