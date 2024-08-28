#include "rect.h"

auto operator==(const Point &lhs, const Point &rhs) -> bool {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

auto operator<<(std::ostream &os, const Point &v) -> std::ostream & {
  return os << "Point{.x = " << v.x << ", .y = " << v.y << "}";
}

auto operator==(const Rect &lhs, const Rect &rhs) -> bool {
  return lhs.lft == rhs.lft && lhs.top == rhs.top && lhs.rgt == rhs.rgt &&
         lhs.bot == rhs.bot;
}

auto operator<<(std::ostream &os, const Rect &r) -> std::ostream & {
  return os << "Rect{.lft = " << r.lft << ", .top = " << r.top
            << ", .rgt = " << r.rgt << ", .bot = " << r.bot << "}";
}

auto operator<<(std::ostream &os, const Bounds &b) -> std::ostream & {
  os << "Bounds{";
  for (const auto &r : b) {
    os << r << ", ";
  }
  return os << "}";
}
