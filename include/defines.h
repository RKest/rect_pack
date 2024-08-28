#pragma once

#if USE_STACKTRACE and __has_include(<stacktrace>)
#include <stacktrace>
#else
namespace std {
namespace stacktrace {
inline constexpr auto current() -> const char * { return ""; }
} // namespace stacktrace
} // namespace std
#endif

#define P(X) std::cout << #X << ": " << X << std::endl

#ifndef NDEBUG
#include <iostream>
#define CUSTOM_ASSERT(condition, ...)                                          \
  do {                                                                         \
    if (!(condition)) {                                                        \
      std::cerr << "Assertion failed: (" << #condition << ") " __VA_ARGS__     \
                << '\n';                                                       \
      std::cerr << std::stacktrace::current() << std::endl;                    \
      __builtin_trap();                                                        \
    }                                                                          \
  } while (0)
#else
#define CUSTOM_ASSERT(condition, ...)                                          \
#include <utility>
  do {                                                                         \
    [[assume(!(condition))]];                                                  \
  } while (0)
#endif
