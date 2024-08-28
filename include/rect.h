#pragma once

#include "defines.h"

#include <ostream>

#if __has_include("raylib.h")
#include "raylib.h"
#define HAS_RAYLIB
#endif

constexpr auto ORIENTATON_MASK = 0b10;
constexpr auto HORIZONTAL = 0b00;
constexpr auto VERTICAL = 0b10;

struct Point {
  float x;
  float y;

  constexpr auto center() const -> Point {
    return Point{
        .x = x / 2,
        .y = y / 2,
    };
  }

#ifdef HAS_RAYLIB
  [[nodiscard]] constexpr auto raylib() const -> Vector2 { return {x, y}; }
#endif
};

struct Rect {
  float lft;
  float top;
  float rgt;
  float bot;

  constexpr auto w() const -> float { return rgt - lft; }
  constexpr auto h() const -> float { return bot - top; }
  constexpr auto tl() const -> Point { return {lft, top}; }
  constexpr auto tr() const -> Point { return {rgt, top}; }
  constexpr auto bl() const -> Point { return {lft, bot}; }
  constexpr auto br() const -> Point { return {rgt, bot}; }
  constexpr auto area() const -> float { return w() * h(); }

  constexpr auto center() const -> Point {
    return {lft + w() / 2, top + h() / 2};
  }

  constexpr auto is_point_inside(Point point) const -> bool {
    return lft < point.x && point.x < rgt && top < point.y && point.y < bot;
  }

  constexpr auto does_overlap(const Rect &o) const -> bool {
    return o.rgt > lft && rgt > o.lft && o.bot > top && bot > o.top;
  }

  friend auto operator==(const Rect &lhs, const Rect &rhs) -> bool;
  friend auto operator<<(std::ostream &os, const Rect &r) -> std::ostream &;

#ifdef HAS_RAYLIB
  [[nodiscard]] constexpr auto raylib() const -> Rectangle {
    return {lft, top, w(), h()};
  }
#endif
};

using Bounds = std::vector<Rect>;

auto operator==(const Point &lhs, const Point &rhs) -> bool;
auto operator<<(std::ostream &os, const Point &v) -> std::ostream &;

auto operator<<(std::ostream &os, const Bounds &b) -> std::ostream &;

constexpr auto operator+(const Point &lhs, const Point &rhs) -> Point {
  return {lhs.x + rhs.x, lhs.y + rhs.y};
}
constexpr auto operator-(const Point &lhs, const Point &rhs) -> Point {
  return {lhs.x - rhs.x, lhs.y - rhs.y};
}
constexpr auto operator*(const Point &lhs, float scalar) -> Point {
  return {lhs.x * scalar, lhs.y * scalar};
}
