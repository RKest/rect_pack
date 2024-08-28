#include "api.h"
#include "cloud.h"

#include "polygon.h"
#include "qtree.h"
#include "raylib.h"
#include "rect.h"

#include <chrono>
#include <functional>

using std::ranges::sort;
using namespace combinators;

constexpr int SCREEN_W = 3200;
constexpr int SCREEN_H = 1800;
constexpr Point DIMS{_float(SCREEN_W), _float(SCREEN_H)};

const std::array colors = {
    Color{128, 0, 0, 255},     // Dark Red
    Color{0, 128, 0, 255},     // Dark Green
    Color{0, 0, 128, 255},     // Dark Blue
    Color{128, 128, 0, 255},   // Dark Yellow
    Color{0, 128, 128, 255},   // Dark Cyan
    Color{128, 0, 128, 255},   // Dark Purple
    Color{128, 128, 128, 255}, // Gray
    Color{255, 107, 107, 255}, // Soft Red
    Color{255, 159, 67, 255},  // Soft Orange
    Color{254, 202, 87, 255},  // Mellow Yellow
    Color{72, 219, 251, 255},  // Sky Blue
    Color{29, 209, 161, 255},  // Mint Green
    Color{108, 92, 231, 255},  // Soft Purple
    Color{162, 155, 254, 255}, // Lavender
    Color{225, 112, 85, 255},  // Coral
    Color{241, 148, 138, 255}, // Peach
    Color{0, 206, 201, 255},   // Turquoise
    Color{255, 184, 184, 255}, // Light Pink
    Color{72, 126, 176, 255},  // Steel Blue
    Color{95, 39, 205, 255},   // Royal Purple
    Color{87, 101, 116, 255},  // Slate Gray
    Color{209, 204, 192, 255}, // Light Taupe
};

auto random_rectangle() -> PolygonE {
  auto w = static_cast<float>(GetRandomValue(20, 50));
  auto s = static_cast<float>(GetRandomValue(20, 50)) / 100.F;
  auto lft = static_cast<float>(GetRandomValue(0, 2400));
  auto top = static_cast<float>(GetRandomValue(0, 1400));
  Rect r{
      .lft = lft,
      .top = top,
      .rgt = lft + w,
      .bot = top + std::round(w * s),
  };
  return PolygonE{{{r}}};
}

constexpr static auto n_rects = 500;
constexpr static auto n_groups = 4;

auto random_index_pair_gen() {
  return [i = n_groups]() mutable {
    return IndexPair{
        .src = static_cast<std::size_t>(GetRandomValue(0, n_groups - 1)),
        .dst = static_cast<std::size_t>(i++),
    };
  };
}

auto randomize_color(Color c) -> Color {
  return Color{static_cast<unsigned char>(c.r + GetRandomValue(0, 50)),
               static_cast<unsigned char>(c.g + GetRandomValue(0, 50)),
               static_cast<unsigned char>(c.b + GetRandomValue(0, 50)), c.a};
}

int main() {
  SetRandomSeed(69420);

  std::vector<Polygon> rects(n_rects);
  std::vector<IndexPair> indices(n_rects - n_groups);
  std::vector<float> tolerances(n_rects, .0F);
  std::ranges::generate(rects, random_rectangle);
  std::ranges::generate(indices, random_index_pair_gen());
  sort(rects, _gt_, &PolygonE::area);

  auto start = std::chrono::steady_clock::now();
  auto positions = place(rects, indices, tolerances, DIMS);
  for (auto &&[pos, rect] : zip(positions, rects)) {
    for (auto &b : rect.rects) {
      b.rgt += pos.x;
      b.bot += pos.y;
      b.lft += pos.x;
      b.top += pos.y;
    }
  }
  auto end = std::chrono::steady_clock::now();
  auto elapsed = duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "Elapsed time: " << elapsed << std::endl;

  InitWindow(SCREEN_W, SCREEN_H, "raylib [core] example - basic window");

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    SetRandomSeed(69420);
    BeginDrawing();
    ClearBackground(RAYWHITE);

    for (auto &&[src, _] : indices) {
      for (auto &r : rects.at(src).rects) {
        DrawRectangleRec(r.raylib(), colors[src]);
      }
    }

    for (auto &&[src, dst] : indices) {
      auto c = randomize_color(colors[src]);
      for (auto &r : rects.at(dst).rects) {
        DrawRectangleRec(r.raylib(), c);
      }
    }

    /*for (auto &s : spirals | ranges::views::take(1)) {*/
    /*  DrawLineStrip(reinterpret_cast<Vector2 *>(s.data.data()),
     * s.data.size(),*/
    /*                BLACK);*/
    /*}*/

    EndDrawing();
  }
  CloseWindow();

  std::exit(0);
}
