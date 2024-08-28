#include "api.h"
#include "cloud.h"

#include <emscripten.h>
#include <emscripten/bind.h>

EMSCRIPTEN_BINDINGS(rp) {
  emscripten::value_object<Point>("Point")
    .field("x", &Point::x)
    .field("y", &Point::y);

  emscripten::value_object<Rect>("Rect")
    .field("lft", &Rect::lft)
    .field("top", &Rect::top)
    .field("rgt", &Rect::rgt)
    .field("bot", &Rect::bot);

  emscripten::value_object<IndexPair>("IndexPair")
    .field("src", &IndexPair::src)
    .field("dst", &IndexPair::dst);

  emscripten::value_object<Polygon>("Polygon")
    .field("rects", &Polygon::rects);

  emscripten::register_vector<Rect>("Bounds");
  emscripten::register_vector<float>("FloatVec");
  emscripten::register_vector<Polygon>("Polygons");
  emscripten::register_vector<IndexPair>("Indices");
  emscripten::register_vector<Point>("Points");

  emscripten::function("place", &place);
}
