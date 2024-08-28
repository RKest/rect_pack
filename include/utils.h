#pragma once

#include "defines.h"
#include "rect.h"

constexpr auto norm = [](Point a, Point b) -> float {
  auto d = a - b;
  return hypotf(d.x, d.y);
};

constexpr auto round_pairwise_transform = [](auto f, auto cont) {
  return concat(zip_with(f, single(cont.back()), single(cont.front())),
                zip_with(f, cont, cont | ranges::views::drop(1)));
};

constexpr auto edge_angle = [](Point start, Point end) -> float {
  return std::atan2(end.y - start.y, end.x - start.x);
};

constexpr auto lerp = [](Point a, Point b, float t) -> Point {
  return {
      .x = std::lerp(a.x, b.x, t),
      .y = std::lerp(a.y, b.y, t),
  };
};

constexpr auto cart_to_polar(Point cart) -> Point {
  return {
      std::sqrt(cart.x * cart.x + cart.y * cart.y),
      std::atan2(cart.y, cart.x),
  };
}

constexpr auto polar_to_cart(Point polar) -> Point {
  return {
      polar.x * std::cos(polar.y),
      polar.x * std::sin(polar.y),
  };
}
