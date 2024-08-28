#include "polygon.h"
#include "spiral.h"

#include <boost/ut.hpp>
#include <cmath>

using namespace boost::ut;

int main() {
  "test_spiral"_test = [] {
    std::vector<Point> data = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    Spiral s{data};

    expect(eq(*std::next(s.begin(), 0), data[0]));
    expect(eq(*std::next(s.begin(), 1), data[1]));
    expect(eq(*std::next(s.begin(), 2), data[2]));
    expect(eq(*std::next(s.begin(), 3), data[3]));
    expect(4_i == s.size());

    s.erase(s.begin() + 2);
    expect(3_i == s.size());

    expect(eq(*std::next(s.begin(), 0), data[1]));
    expect(eq(*std::next(s.begin(), 1), data[0]));
    expect(eq(*std::next(s.begin(), 2), data[3]));

    s.erase(s.begin() + 2);
    expect(2_i == s.size());

    expect(eq(*std::next(s.begin(), 0), data[0]));
    expect(eq(*std::next(s.begin(), 1), data[1]));

    s.erase(s.begin());
    expect(1_i == s.size());

    expect(eq(*std::next(s.begin(), 0), data[1]));
  };

  "test_spiral_non_invalidating"_test = [] {
    std::vector<Point> data = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    Spiral s{data};

    expect(eq(*std::next(s.begin(), 0), data[0]));
    expect(eq(*std::next(s.begin(), 1), data[1]));
    s.erase(s.begin() + 2);
    expect(eq(*std::next(s.begin(), 2), data[3]));
    expect(3_i == s.size());
  };

  "test_rect_intersection"_test = [] {
    Rect r1{0, 0, 10, 10};
    Rect r2{5, 5, 15, 15};
    expect(r1.does_overlap(r2));
  };

  "test_polygon_simplification_1"_test = [] {
    Polygon poly{{
        Rect{.lft = 0, .top = 0, .rgt = 5, .bot = 5},
        Rect{.lft = 5, .top = 0, .rgt = 10, .bot = 4},
        Rect{.lft = 10, .top = 1, .rgt = 15, .bot = 10},
    }};
    poly.simplify(/* threshold */ 1.01F);

    expect(eq(poly.rects, std::vector{
                              Rect{.lft = 0, .top = 0, .rgt = 10, .bot = 5},
                              Rect{.lft = 10, .top = 0, .rgt = 15, .bot = 10},
                          }));
  };

  "test_polygon_simplification_2"_test = [] {
    Polygon poly{{
        Rect{.lft = 0, .top = 1, .rgt = 5, .bot = 5},
        Rect{.lft = 5, .top = 2, .rgt = 10, .bot = 4},
        Rect{.lft = 10, .top = 1, .rgt = 15, .bot = 6},
    }};
    poly.simplify(/* threshold */ 1.01F);

    expect(eq(poly.rects, std::vector{
                              Rect{.lft = 0, .top = 1, .rgt = 15, .bot = 6},
                          }));
  };

  "test_polygon_outside_edges"_test = [] {
    Polygon poly{{
        Rect{.lft = 0, .top = 0, .rgt = 5, .bot = 5},
        Rect{.lft = 6, .top = 1, .rgt = 10, .bot = 5},
        Rect{.lft = 11, .top = 1, .rgt = 15, .bot = 7},
        Rect{.lft = 16, .top = 0, .rgt = 18, .bot = 5},
    }};
    expect(eq(poly.outside_edge_points(), std::vector{
                                              Point{.x = 0, .y = 0},
                                              Point{.x = 5, .y = 0},
                                              Point{.x = 5, .y = 1},
                                              Point{.x = 15, .y = 1},
                                              Point{.x = 15, .y = 0},
                                              Point{.x = 18, .y = 0},
                                              Point{.x = 18, .y = 5},
                                              Point{.x = 16, .y = 5},
                                              Point{.x = 16, .y = 7},
                                              Point{.x = 11, .y = 7},
                                              Point{.x = 11, .y = 5},
                                              Point{.x = 0, .y = 5},
                                          }));
  };

  "test_angle"_test = [] {
    Point p = {1, 1};
    Point q = {2, 2};
    float expected = M_PI_4;
    float diff = fabsf(expected - edge_angle(p, q));
    expect(lt(diff, 0.001F));
  };

  "test_edge_isect"_test = [] {
    Edge e1{{1, 1}, {3, 4}};
    Edge e2{{3, 1}, {1, 4}};
    Point isect;
    expect(e1.intersection(e2, isect));
    expect(eq(isect, Point{2, 2.5}));
  };

  "test_polygon_isect"_test = [] {
    PolygonE poly{{{Rect{.lft = 0, .top = 0, .rgt = 4, .bot = 4}}}};
    expect(poly.center == Point{2, 2});

    auto expected = Point{.x = 3.1547, .y = 4};
    Point closes_isect;
    if (not expect(poly.closest_isect(M_PI / 3.F, closes_isect)))
      return;

    auto diff = closes_isect - expected;

    expect(diff.x < 0.001F);
    expect(diff.y < 0.001F);
  };
}
