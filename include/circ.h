#pragma once

#include "combinators.hpp"
#include "defines.h"
#include "rect.h"
#include "utils.h"

struct Slice;
struct Circ {
  Point center;
  float radius;

  auto split(std::span<const float> weights) const -> std::vector<Slice>;
};

template <std::size_t Res> struct SlicePoints {
  std::array<Point, Res> data;
  std::span<Point> outside;
};

struct Slice {
  Circ circ;
  float start_rad{};
  float end_rad{};

  template <std::size_t Res> inline auto points() const -> SlicePoints<Res>;
  constexpr auto centroid(float rad_mult = 1.F) const -> Point;
};

struct IndexPair {
  std::size_t src;
  std::size_t dst;
};

inline auto
Circ::split(std::span<const float> weights) const -> std::vector<Slice> {
  CUSTOM_ASSERT(weights.size() >= 1);

  std::vector<Slice> result;
  result.reserve(weights.size());
  const auto total_weight = accumulate(weights, 0.F);
  float theta = 0.F;
  for (auto w : weights) {
    float start_r = theta;
    theta += w / total_weight * M_PI * 2.F;
    result.push_back({*this, start_r, theta});
  }
  return result;
}

template <std::size_t Res>
inline auto Slice::points() const -> SlicePoints<Res> {
  CUSTOM_ASSERT(start_rad < end_rad);

  constexpr auto floor = _b(_size_t, floorf);

  const auto delta = end_rad - start_rad;
  const auto outside_len = delta * circ.radius;
  const auto slice_circumf = 2.F * circ.radius + outside_len;
  auto outside_res = floor((outside_len / slice_circumf) * Res);
  outside_res = _even(outside_res) ? outside_res : outside_res + 1;
  const auto radius_res = (Res - outside_res) >> 1; // divide by 2;
  outside_res -= 1;

  SlicePoints<Res> result;
  std::size_t next_index = 0;
  auto push_back = [&](Point p) { result.data[next_index++] = p; };

  const auto start_tip = circ.center + polar_to_cart({circ.radius, start_rad});
  const auto end_tip = circ.center + polar_to_cart({circ.radius, end_rad});

  push_back(circ.center);
  for (size_t i = 1; i < radius_res; ++i) {
    const float t = i / _float(radius_res);
    const Point p = lerp(circ.center, start_tip, t);
    push_back(p);
  }
  const auto outside_start_index = next_index;
  push_back(start_tip);
  for (size_t i = 1; i < outside_res; ++i) {
    const float t = i / _float(outside_res);
    const float α = std::lerp(start_rad, end_rad, t);
    push_back(circ.center + polar_to_cart({circ.radius, α}));
  }
  push_back(end_tip);
  const auto outside_size = next_index - outside_start_index;
  for (size_t i = 1; i < radius_res; ++i) {
    const float t = i / _float(radius_res);
    const Point p = lerp(end_tip, circ.center, t);
    push_back(p);
  }
  push_back(circ.center);
  const auto points = std::span{result.data};
  result.outside = points.subspan(outside_start_index, outside_size);

  CUSTOM_ASSERT(result.outside.front() == start_tip);
  CUSTOM_ASSERT(result.outside.back() == end_tip);

  return result;
}

constexpr auto Slice::centroid(float rad_mult) const -> Point {
  const auto alpha = end_rad - start_rad;
  const auto rad = circ.radius;

  const auto xbar = (2.F / 3.F) * (rad / alpha) * sinf(alpha);
  const auto ybar = (-2.F / 3.F) * (rad / alpha) * (cosf(alpha) - 1.F);

  // rotate coordinates about (0, 0) by startAngle
  const auto x = xbar * cosf(start_rad) - ybar * sinf(start_rad);
  const auto y = ybar * cosf(start_rad) + xbar * sinf(start_rad);

  return circ.center + Point{x, y} * rad_mult;
}
