#pragma once

#include "circ.h"
#include "defines.h"
#include "polygon.h"
#include "qtree.h"
#include "range/v3/algorithm/any_of.hpp"
#include "rect.h"
#include "spiral.h"

inline auto slice_points(Slice slice) -> std::vector<Point> {
  constexpr float rad_inc = (M_PI * 2) / 100;
  CUSTOM_ASSERT(slice.start_rad < slice.end_rad);
  std::vector<Point> result{slice.circ.center};

  while (slice.start_rad < slice.end_rad) {
    result.push_back(slice.circ.center +
                     polar_to_cart({slice.circ.radius, slice.start_rad}));
    slice.start_rad += rad_inc;
  }
  result.push_back(slice.circ.center +
                   polar_to_cart({slice.circ.radius, slice.end_rad}));

  return result;
}

inline auto bounds_points(const Bounds &b) -> std::vector<Point> {
  std::vector<Point> result;
  for (const auto &r : b) {
    result.push_back({r.lft, r.top});
    result.push_back({r.lft, r.bot});
    result.push_back({r.rgt, r.bot});
    result.push_back({r.rgt, r.top});
  }
  return result;
}

inline void make_center_eq(const Point v, Polygon &b) {
  auto c = centroid(b.outside_edge_points());
  auto d = v - c;
  for (auto &r : b.rects) {
    r.lft += d.x;
    r.rgt += d.x;
    r.top += d.y;
    r.bot += d.y;
  }
}

inline auto
make_cloud(std::span<PolygonE> polys, std::span<const IndexPair> indices,
           Point board_dims) -> std::pair<int, std::vector<Spiral>> {
  CUSTOM_ASSERT(!polys.empty());
  CUSTOM_ASSERT(!indices.empty());

  const size_t max_src_inx = max_element(indices, _lt_, &IndexPair::src)->src;
  std::vector<float> areas(max_src_inx + 1);
  for (auto [src, dst] : indices) {
    CUSTOM_ASSERT(src < polys.size());
    CUSTOM_ASSERT(dst < polys.size());
    CUSTOM_ASSERT(src != dst);
    if (areas[src] == 0.F)
      areas[src] = accumulate(polys[src].rects, 0.F, _plus_, &Rect::area);
    areas[src] += accumulate(polys[dst].rects, 0.F, _plus_, &Rect::area);
  }
  CUSTOM_ASSERT(all_of(areas, _gt(0.F)));
  const float total_area = accumulate(areas, 0.F);
  const float radius = std::sqrt(total_area / M_PI);
  const Point center = board_dims.center();
  const Circ circ{center, radius};

  const auto slices = circ.split(areas);
  auto spirals = slices | transform(spiral) | to_vector;
  const auto spirals_cp = spirals;
  CUSTOM_ASSERT(spirals.size() == areas.size());

  const auto padding = radius;
  const auto bbox_lft = std::max(.0F, center.x - radius - padding);
  const auto bbox_top = std::max(.0F, center.y - radius - padding);
  const auto bounding_box = Rect{
      bbox_lft,
      bbox_top,
      std::min(_float(board_dims.x), center.x + radius + padding),
      std::min(_float(board_dims.y), center.y + radius + padding),
  };
  qtree::Qtree quadtree{qtree::Qbound{bounding_box}};
  auto poly_intersects = [&quadtree](const Polygon &p) {
    return ranges::any_of(p.rects, [&quadtree](const Rect &r) {
      return quadtree.rect_intersects(r);
    });
  };
  auto poly_quadtree_insert = [&quadtree](const Polygon &p) {
    for (const Rect &r : p.rects) {
      quadtree.insert(r);
    }
  };

  std::vector<Point> centers(spirals.size());

  int number_placed = 0;
  for (std::size_t src = 0; src <= max_src_inx; ++src) {
    auto &spiral = spirals[src];
    auto &poly = polys[src];
    for (auto it = spiral.begin(); it != spiral.end(); ++it) {
      make_center_eq(*it, poly);
      if (not poly_intersects(poly)) {
        poly_quadtree_insert(poly);
        centers[src] = *it;
        number_placed++;
        break;
      }
    }
  }

  for (auto [src, dst] : indices) {
    CUSTOM_ASSERT(src < spirals.size());
    CUSTOM_ASSERT(src < centers.size());
    auto &spiral = spirals[src];
    auto &poly = polys[dst];
    for (auto it = spiral.begin() + 1; it != spiral.end(); ++it) {
      if (quadtree.point_intersects(*it)) {
        spiral.erase(it);
        continue;
      }
      auto theta = edge_angle(*it, centers[src]);
      Point closest_isect;
      if (not poly.closest_isect(theta, closest_isect)) {
        continue;
      }
      poly.move_by(*it - closest_isect);
      if (not poly_intersects(poly)) {
        poly_quadtree_insert(poly);
        number_placed++;
        break;
      }
    }
  }

  return {number_placed, std::move(spirals_cp)};
}
