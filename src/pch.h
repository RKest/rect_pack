#include <algorithm>
#include <array>
#include <cmath>
#include <combinators.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <limits>
#include <ostream>
#include <range/v3/algorithm/all_of.hpp>
#include <range/v3/algorithm/any_of.hpp>
#include <range/v3/algorithm/max_element.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/enumerate.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/zip.hpp>
#include <ranges>
#include <type_traits>
#include <vector>

#if __has_include(<raylib.h>)
#include <raylib.h>
#endif

using ranges::accumulate;
using ranges::all_of;
using ranges::bind_back;
using ranges::max_element;
using ranges::to_vector;
using ranges::views::zip_with;
using ranges::views::zip;
using ranges::views::transform;
using namespace combinators;
