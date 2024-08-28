#include "defines.h"

#include "circ.h"
#include "rect.h"
#include "utils.h"

inline auto area(std::span<const Point> ps) -> float {
  auto sum = [](Point p, Point q) -> float {
    return (q.x + p.x) * (q.y - p.y);
  };
  auto area_sum = .0F;
  for (size_t i = ps.size() - 1, j = 0; j < ps.size(); i = j++) {
    area_sum += sum(ps[i], ps[j]);
  }
  return std::abs(area_sum / 2.F);
}

inline auto centroid(std::span<const Point> ps) -> Point {
  auto sum = [](Point p, Point q) -> Point {
    return (p + q) * (p.x * q.y - q.x * p.y);
  };
  const float a = 6 * area(ps);
  auto scalars = Point{0, 0};
  for (size_t i = ps.size() - 1, j = 0; j < ps.size(); i = j++) {
    scalars = scalars + sum(ps[i], ps[j]);
  }
  CUSTOM_ASSERT(a > 0);
  return {
      .x = std::abs(scalars.x / a),
      .y = std::abs(scalars.y / a),
  };
}

struct Spiral {
  using iterator = std::vector<Point>::iterator;

  std::vector<Point> data;
  iterator slow = std::begin(data);

  inline auto begin() { return slow; }
  inline auto end() { return std::end(data); }

  inline auto front() -> Point & { return *begin(); }

  inline auto size() -> std::size_t { return std::distance(begin(), end()); }
  inline auto erase(decltype(data)::iterator it) {
    std::iter_swap(it, slow++);
  };
};

inline auto spiral(Slice slice) -> Spiral {
  constexpr auto resolution = 20;
  constexpr auto padding_mult = 1.2F;
  constexpr auto padding_resolution = _int(resolution * padding_mult);
  constexpr auto sparcity = 0;
  constexpr auto slice_res = 100;

  auto points = slice.points<slice_res>();
  auto center = slice.centroid();
  std::vector<Point> result;
  result.reserve(slice_res * (resolution + padding_resolution));
  auto t = .0F;
  for (int i = 0; i < resolution; ++i) {
    const auto leap = std::max(sparcity - i, 1);
    const auto step = 1 / (slice_res * _float(resolution) / leap);
    for (int j = 0; j < points.data.size(); j += leap) {
      const auto &p = points.data[j];
      result.push_back(lerp(center, p, t));
      t += step;
    }
  }
  
  const auto step = 1 / _float(padding_resolution);
  for (int i = 0; i < padding_resolution; ++i) {
    for (auto p : points.outside) {
      result.push_back(lerp(center, p, t));
    }
    t += step;
  }

  return Spiral{std::move(result)};
}
