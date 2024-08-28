#pragma once

#include "defines.h"
#include "rect.h"
#include "utils.h"

struct Edge {
  Point p, q;

  friend auto operator==(const Edge &lhs, const Edge &rhs) -> bool = default;
  friend auto operator<<(std::ostream &os, const Edge &e) -> std::ostream & {
    return os << "Edge{" << e.p << ", " << e.q << "}";
  }

  auto intersection(const Edge &other, Point &out) const -> bool;
};

struct Polygon {
  std::vector<Rect> rects;

  void simplify(float threshold);
  auto area() const -> float;
  auto outside_edge_points() -> std::vector<Point>;
};

struct PolygonE : Polygon {
  std::vector<Point> edge_points = outside_edge_points();
  Point center = centroid();

  auto closest_isect(float theta, Point &out) const -> bool;
  auto centroid() -> Point;
  void move_by(Point vector);
};

inline auto Polygon::area() const -> float {
  return accumulate(rects, 0.F, _plus_, &Rect::area);
}

inline auto Edge::intersection(const Edge &other, Point &out) const -> bool {
  const float s1_x = q.x - p.x;
  const float s1_y = q.y - p.y;
  const float s2_x = other.q.x - other.p.x;
  const float s2_y = other.q.y - other.p.y;

  const float s = (-s1_y * (p.x - other.p.x) + s1_x * (p.y - other.p.y)) /
                  (-s2_x * s1_y + s1_x * s2_y);
  const float t = (s2_x * (p.y - other.p.y) - s2_y * (p.x - other.p.x)) /
                  (-s2_x * s1_y + s1_x * s2_y);

  if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
    out.x = p.x + (t * s1_x);
    out.y = p.y + (t * s1_y);
    return true;
  }
  return false;
}

inline void Polygon::simplify(float threshold) {
  // |x - y| < threshold
  const auto withinThresh = [threshold](float a, float b) {
    return fabsf(a - b) < threshold;
  };
  const auto top_eq = [withinThresh](const Rect &a, const Rect &b) {
    return withinThresh(a.top, b.top);
  };
  const auto bot_eq = [withinThresh](const Rect &a, const Rect &b) {
    return withinThresh(a.bot, b.bot);
  };
  auto r1 = begin(rects);
  auto r2 = begin(rects) + 1;
  for (; r2 < end(rects); r1 = r2++) {
    auto teq = top_eq(*r1, *r2);
    auto beq = bot_eq(*r1, *r2);

    if (teq && beq) {
      auto min_t = std::min(r1->top, r2->top);
      auto max_b = std::max(r1->bot, r2->bot);
      for (auto e = end(rects), r3 = next(r2);
           r3 != e && top_eq(*r1, *r3) && bot_eq(*r1, *r3); r2 = r3++) {
        min_t = std::min(min_t, r3->top);
        max_b = std::max(max_b, r3->bot);
      }
      r1->top = min_t;
      r1->bot = max_b;
      r1->rgt = r2->rgt;
      rects.erase(r1 + 1, r2 + 1);
    } else if (teq) {
      r1->top = std::min(r1->top, r2->top);
    } else if (beq) {
      r1->bot = std::max(r1->bot, r2->bot);
    }
  }
  if (rects.size() > 1) {
    auto &r1 = *next(rects.rbegin());
    auto &r2 = rects.back();
    if (top_eq(r1, r2)) {
      r2.top = std::min(r1.top, r2.top);
    }
    if (bot_eq(r1, r2)) {
      r2.bot = std::max(r1.bot, r2.bot);
    }
  }
}

inline auto Polygon::outside_edge_points() -> std::vector<Point> {
  if (rects.empty()) {
    return {};
  }
  const Point first = rects.front().bl();
  Point curr_point = rects.front().tl();
  std::vector<Point> result{curr_point};
  Rect *next, *curr = &rects.front();
  enum Corner { BL, TL, BR, TR } curr_corner = TL;
  auto add_point = [&](Point p, Corner c) {
    result.emplace_back(p);
    curr_point = p;
    curr_corner = c;
  };
  int depth = 1000;
  while (curr_point != first && --depth) {
    switch (curr_corner) {
    case TL:
      for (next = curr + 1; next <= &rects.back() && curr->top == next->top;
           curr = next, next = curr + 1)
        ;
      add_point(curr->tr(), TR);
      break;
    case BR:
      for (next = curr - 1; next >= &rects.front() && curr->bot == next->bot;
           curr = next, next = curr - 1)
        ;
      add_point(curr->bl(), BL);
      break;
    case BL:
      next = curr - 1;
      if (next < &rects.front()) {
        add_point(curr->tl(), TL);
      } else {
        add_point(Point{curr->lft, next->bot}, BR);
        curr = next;
      }
      break;
    case TR:
      next = curr + 1;
      if (next > &rects.back()) {
        add_point(curr->br(), BR);
      } else {
        add_point(Point{curr->rgt, next->top}, TL);
        curr = next;
      }
      break;
    }
  }
  CUSTOM_ASSERT(depth > 0);
  return result;
}

inline void PolygonE::move_by(Point vector) {
  for (auto &r : rects) {
    r.lft += vector.x;
    r.rgt += vector.x;
    r.top += vector.y;
    r.bot += vector.y;
  }
  for (auto &p : edge_points) {
    p = p + vector;
  }
  center = center + vector;
}

inline auto PolygonE::closest_isect(float theta, Point &out) const -> bool {
  constexpr float far = 10000.F;
  Point closest{nanf(""), nanf("")};
  float min_dist = std::numeric_limits<float>::max();

  Edge ray{center, center + Point{cosf(theta) * far, sinf(theta) * far}};
  for (int i = edge_points.size() - 1, j = 0; j < edge_points.size(); i = j++) {
    Edge edge{edge_points[i], edge_points[j]};
    if (Point isect; edge.intersection(ray, isect)) {
      float dist = norm(isect, center);
      if (dist < min_dist) {
        min_dist = dist;
        closest = isect;
      }
    }
  }
  if (!std::isnan(closest.x)) {
    out = closest;
    return true;
  }
  return false;
}

inline auto PolygonE::centroid() -> Point {
  if (edge_points.empty()) {
    return {};
  }
  auto sum = [](Point p, Point q) -> Point {
    return (p + q) * (p.x * q.y - q.x * p.y);
  };
  const float a6 = 6 * area();
  auto scalars = Point{0, 0};
  for (size_t i = edge_points.size() - 1, j = 0; j < edge_points.size();
       i = j++) {
    scalars = scalars + sum(edge_points[i], edge_points[j]);
  }
  CUSTOM_ASSERT(a6 > 0);
  return {
      .x = std::abs(scalars.x / a6),
      .y = std::abs(scalars.y / a6),
  };
}
