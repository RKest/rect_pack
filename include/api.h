#pragma once

#include "cloud.h"
#include "rect.h"

inline auto place(std::vector<Polygon> skills, std::vector<IndexPair> indices,
                  std::vector<float> tolerances,
                  Point board_dims) -> std::vector<Point> {
  std::vector<PolygonE> bounds;
  bounds.reserve(skills.size());
  for (auto [skill, tol] : zip(skills, tolerances)) {
    bounds.emplace_back(skill);
    bounds.back().simplify(tol);
  }
  auto [placed, _] = make_cloud(bounds, indices, board_dims);
  std::vector<Point> result;
  for (auto&& [bound, skill] : ranges::views::zip(bounds, skills)) {
    result.push_back(bound.rects.front().tl() - skill.rects.front().tl());
  }
  return result;
}
