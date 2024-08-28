#pragma once

#include <span>
#include <type_traits>
#include <vector>

template <typename T, size_t N> struct SmallList {
  std::span<T, N> values;
  std::vector<T> overflow;
  std::size_t size = 0;

  auto is_empty() -> bool { return size == 0; }

  void push_back(T const &value) {
    if (size < N) {
      values[size++] = value;
    } else {
      overflow.push_back(value);
      size++;
    }
  }

  template <typename... Args> void emplace_back(Args &&...args) {
    if (size < N) {
      values[size++] = T{std::forward<Args>(args)...};
    } else {
      overflow.emplace_back(std::forward<Args>(args)...);
      size++;
    }
  }

  auto pop_back() -> T {
    if (size < N) {
      return values[--size];
    } else {
      auto value = std::move(overflow.back());
      overflow.pop_back();
      size--;
      return value;
    }
  }

  ~SmallList() = default;
  ~SmallList()
    requires(not std::is_trivially_destructible_v<T>)
  {
    for (auto &value : overflow) {
      value.~T();
    }
  }
};
