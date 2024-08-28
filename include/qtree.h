#pragma once

#include "rect.h"
#include "small_list.h"

#include <deque>
#include <limits>

namespace qtree {

constexpr static char8_t qtree_capacity = 8;

struct Qtree;
struct Qbound;
struct Qnode;

using QvalueArray = std::array<Rect, qtree_capacity>;
using Qsubdivision = std::array<Qnode, 4>;

enum Qquadrant : uint16_t {
  TopLft,
  TopRgt,
  BotLft,
  BotRgt,
};

struct Qnode {
  uint16_t ptr = -1; // Pointer to the data, or children if it's not a leaf
  uint16_t size{};   // std::numeric_limits::max() when this is not a leaf node

  auto init_leaf(Qtree &parent) -> QvalueArray &;
  auto values(Qtree &parent) const -> QvalueArray &;
  auto children(Qtree &parent) const -> Qsubdivision &;
  void insert(Qbound const &bound, Rect const &rect, Qtree &parent);

  constexpr auto is_leaf() const -> bool {
    return size != std::numeric_limits<decltype(size)>::max();
  }
};

struct Qbound {
  Rect rect;

  template <Qquadrant quadrant> constexpr auto divide() const -> Qbound;
};

struct Qeval {
  Qnode node;
  Qbound bound;
};

struct Qinsert {
  Qnode *node;
  Qbound bound;
};

struct Qtree {
  Qbound root_bound;

  Qnode root{0, 0};

  std::deque<Qsubdivision> children;
  std::deque<QvalueArray> values{1};

  std::array<Qinsert, 128> insert_list_data;
  std::array<Qeval, 128> eval_list_data;

  void insert(const Rect &rect);
  void split_node(Qnode *node, Qbound const &bound, Rect const &r);
  [[nodiscard]] auto rect_intersects(const Rect &rect) -> bool;
  template <Qquadrant quadrant>
  [[nodiscard]] auto r_intersects(Rect const &r, Qbound const &b,
                                  Qnode n) -> bool;
  [[nodiscard]] auto point_intersects(Point p) -> bool;
  template <Qquadrant quadrant>
  [[nodiscard]] auto p_intersects(Point p, Qbound const &b, Qnode n) -> bool;
  [[nodiscard]] auto bounds() -> std::vector<Qbound>;
};

inline auto Qnode::init_leaf(Qtree &parent) -> QvalueArray & {
  ptr = parent.values.size();
  return parent.values.emplace_back();
}

inline auto Qnode::children(Qtree &parent) const -> Qsubdivision & {
  return parent.children.at(ptr);
}

inline auto Qnode::values(Qtree &parent) const -> QvalueArray & {
  return parent.values.at(ptr);
}

template <Qquadrant quadrant> constexpr auto Qbound::divide() const -> Qbound {
  const float half_rgt = rect.lft + rect.w() / 2;
  const float half_bot = rect.top + rect.h() / 2;
  if constexpr (quadrant == TopLft) {
    return {Rect{rect.lft, rect.top, half_rgt, half_bot}};
  } else if constexpr (quadrant == TopRgt) {
    return {Rect{half_rgt, rect.top, rect.rgt, half_bot}};
  } else if constexpr (quadrant == BotLft) {
    return {Rect{rect.lft, half_bot, half_rgt, rect.bot}};
  } else if constexpr (quadrant == BotRgt) {
    return {Rect{half_rgt, half_bot, rect.rgt, rect.bot}};
  } else {
    static_assert(false, "Unreachable");
  }
}

inline void Qtree::insert(const Rect &r) {
  if (not root_bound.rect.does_overlap(r)) [[unlikely]] {
    return;
  }

  SmallList stack{std::span{insert_list_data}};
  stack.emplace_back(&root, root_bound);
  while (not stack.is_empty()) {
    auto [top_node, top_bound] = stack.pop_back();

    if (top_node->is_leaf()) {
      if (top_node->size == qtree_capacity) {
        split_node(top_node, top_bound, r);
      } else {
        top_node->values(*this)[top_node->size++] = r;
      }
    } else {
      auto &child_nodes = top_node->children(*this);
      auto emplace = [&]<Qquadrant qd>(std::integral_constant<Qquadrant, qd>) {
        const Qbound div = top_bound.divide<qd>();
        Qnode &qnode = child_nodes.at(qd);
        if (div.rect.does_overlap(r)) {
          stack.emplace_back(&qnode, div);
        }
      };
      emplace(std::integral_constant<Qquadrant, TopLft>{});
      emplace(std::integral_constant<Qquadrant, TopRgt>{});
      emplace(std::integral_constant<Qquadrant, BotLft>{});
      emplace(std::integral_constant<Qquadrant, BotRgt>{});
    }
  }
}

inline void Qtree::split_node(Qnode *node, Qbound const &bound, Rect const &r) {
  QvalueArray &values = node->values(*this);
  const Qbound div = bound.divide<TopLft>();
  node->ptr = children.size();
  Qsubdivision &node_children = children.emplace_back();
  node->size = -1;
  auto append = [&]<Qquadrant qd>(std::integral_constant<Qquadrant, qd>) {
    const Qbound div = bound.divide<qd>();
    auto &node = node_children[qd];
    auto &node_values = node.init_leaf(*this);
    for (auto &v : values) {
      if (div.rect.does_overlap(v)) {
        node_values[node.size++] = v;
      }
    }
    if (div.rect.does_overlap(r)) {
      if (node.size == qtree_capacity) {
        split_node(&node, div, r);
      } else {
        node_values[node.size++] = r;
      }
    }
  };
  append(std::integral_constant<Qquadrant, TopLft>{});
  append(std::integral_constant<Qquadrant, TopRgt>{});
  append(std::integral_constant<Qquadrant, BotLft>{});
  append(std::integral_constant<Qquadrant, BotRgt>{});
}

inline auto Qtree::rect_intersects(const Rect &r) -> bool {
  if (not root_bound.rect.does_overlap(r)) [[unlikely]] {
    return false;
  }

  SmallList stack{std::span{eval_list_data}};
  stack.emplace_back(root, root_bound);
  while (not stack.is_empty()) {
    auto [top_node, top_bound] = stack.pop_back();

    if (top_node.is_leaf()) {
      for (uint16_t i = 0; i < top_node.size; ++i) {
        if (top_node.values(*this)[i].does_overlap(r)) {
          return true;
        }
      }
    } else {
      const auto &child_nodes = top_node.children(*this);
      
      auto tld = top_bound.divide<TopLft>();
      auto trd = top_bound.divide<TopRgt>();
      auto bld = top_bound.divide<BotLft>();
      auto brd = top_bound.divide<BotRgt>();

      if (tld.rect.does_overlap(r)) {
        stack.emplace_back(child_nodes.at(TopLft), tld);
      }
      if (trd.rect.does_overlap(r)) {
        stack.emplace_back(child_nodes.at(TopRgt), trd);
      }
      if (bld.rect.does_overlap(r)) {
        stack.emplace_back(child_nodes.at(BotLft), bld);
      }
      if (brd.rect.does_overlap(r)) {
        stack.emplace_back(child_nodes.at(BotRgt), brd);
      }
    }
  }
  return false;
}

inline auto Qtree::point_intersects(Point p) -> bool {
  if (not root_bound.rect.is_point_inside(p)) [[unlikely]] {
    return false;
  }

  SmallList stack{std::span{eval_list_data}};
  stack.emplace_back(root, root_bound);
  while (not stack.is_empty()) {
    auto [top_node, top_bound] = stack.pop_back();

    if (top_node.is_leaf()) {
      for (uint16_t i = 0; i < top_node.size; ++i) {
        if (top_node.values(*this)[i].is_point_inside(p)) {
          return true;
        }
      }
    } else {
      auto tld = top_bound.divide<TopLft>();
      auto trd = top_bound.divide<TopRgt>();
      auto bld = top_bound.divide<BotLft>();
      auto brd = top_bound.divide<BotRgt>();

      const auto &child_nodes = top_node.children(*this);
      if (tld.rect.is_point_inside(p)) {
        stack.emplace_back(child_nodes.at(TopLft), tld);
      }
      if (trd.rect.is_point_inside(p)) {
        stack.emplace_back(child_nodes.at(TopRgt), trd);
      }
      if (bld.rect.is_point_inside(p)) {
        stack.emplace_back(child_nodes.at(BotLft), bld);
      }
      if (brd.rect.is_point_inside(p)) {
        stack.emplace_back(child_nodes.at(BotRgt), brd);
      }
    }
  }
  return false;
}

template <Qquadrant quadrant>
inline void bound_recurse(Qbound b, Qnode node, std::vector<Qbound> &result,
                          Qtree &parent) {
  const Qbound bound = b.divide<quadrant>();
  if (not node.is_leaf()) {
    bound_recurse<TopLft>(bound, node.children(parent).at(TopLft), result,
                          parent);
    bound_recurse<TopRgt>(bound, node.children(parent).at(TopRgt), result,
                          parent);
    bound_recurse<BotLft>(bound, node.children(parent).at(BotLft), result,
                          parent);
    bound_recurse<BotRgt>(bound, node.children(parent).at(BotRgt), result,
                          parent);
  } else {
    result.push_back(bound);
  }
}

inline auto Qtree::bounds() -> std::vector<Qbound> {
  std::vector<Qbound> result{root_bound};
  if (children.size() > 0) {
    bound_recurse<TopLft>(root_bound, children.front().at(TopLft), result,
                          *this);
    bound_recurse<TopRgt>(root_bound, children.front().at(TopRgt), result,
                          *this);
    bound_recurse<BotLft>(root_bound, children.front().at(BotLft), result,
                          *this);
    bound_recurse<BotRgt>(root_bound, children.front().at(BotRgt), result,
                          *this);
  }
  return result;
}

} // namespace qtree
