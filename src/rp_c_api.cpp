#if 0
#include "defines.h"

#include "cloud.h"
#include "rect.h"
#include "rp_c_api.h"

namespace {
thread_local std::string LAST_ERROR;
}

const char *rp_get_error() { return LAST_ERROR.c_str(); }

int rp_version() { return 3; }

int rp_pack_rects(RpPosition *out_positions, RpRect *rects, int n_rects,
                  RpIndexPair *indices, int n_indices, RpPosition center) {
  std::vector<Rect> in_rects_vec;
  std::vector<Rect *> out_rects_vec;
  std::vector<IndexPair> indices_vec;
  in_rects_vec.reserve(n_rects);
  out_rects_vec.reserve(n_rects);
  indices_vec.reserve(n_indices);
  for (int i = 0; i < n_rects; ++i) {
    in_rects_vec.emplace_back(rects[i].x, rects[i].y, rects[i].w, rects[i].h);
  }
  for (int i = 0; i < n_indices; ++i) {
    indices_vec.emplace_back(indices[i].src, indices[i].dst);
  }
  Vector2 center_vec{center.x, center.y};
  auto hierarchies = optimize(in_rects_vec, indices_vec, center_vec);
  if (!hierarchies.has_value()) {
    LAST_ERROR = hierarchies.error();
    return -1;
  }
  for (auto &&h : hierarchies.value()) {
    out_rects_vec.push_back(h.parent);
    for (auto &&c : h.children) {
      out_rects_vec.push_back(c);
    }
  }

  std::ranges::sort(out_rects_vec); // Sorting pointers to preserve input order
  for (int i = 0; i < ssize(out_rects_vec); ++i) {
    CUSTOM_ASSERT(out_rects_vec[i]->w == rects[i].w);
    CUSTOM_ASSERT(out_rects_vec[i]->h == rects[i].h);
    out_positions[i] = {out_rects_vec[i]->x, out_rects_vec[i]->y};
  }

  std::cout << "Positions = [";
  for (int i = 0; i < ssize(out_rects_vec); ++i) {
    std::cout << "(" << out_positions[i].x << ", " << out_positions[i].y
              << "), ";
  }
  std::cout << "]\n";

  return ssize(out_rects_vec);
}
#endif
