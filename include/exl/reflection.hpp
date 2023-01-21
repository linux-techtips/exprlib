/*
  Couldn't find a github or a license for this project, so here's where I got
  the sauce:
  https://towardsdev.com/counting-the-number-of-fields-in-an-aggregate-in-c-20-c81aecfd725c
*/

/* Modified by Carter Vavra */

#pragma once

#include <exl/types.hpp>

namespace exl::reflection {

template <typename T>
concept Aggregate = std::is_aggregate_v<T>;

template <typename T, typename... Args>
concept AggregateInit = Aggregate<T> && requires {
  T{std::declval<Args>()...};
};

// NOLINTBEGIN
namespace impl {
struct Universal {
  template <typename T> constexpr operator T &() const noexcept;
  template <typename T> constexpr operator T &&() const noexcept;
};
} // namespace impl
// NOLINTEND

namespace impl {
template <usize I> using IndexedUniversal = Universal;

template <Aggregate T, typename Indices> struct AggregateInitFromIndicies;

template <Aggregate T, usize... Indices>
struct AggregateInitFromIndicies<T, std::index_sequence<Indices...>>
    : std::bool_constant<AggregateInit<T, IndexedUniversal<Indices>...>> {};
} // namespace impl

template <typename T, usize N>
concept AggregateInitWithNArgs = Aggregate<T> &&
    impl::AggregateInitFromIndicies<T, std::make_index_sequence<N>>::value;

template <template <usize> typename Predicate, usize Beg, usize End>
struct BinarySearch;

template <template <usize> typename Predicate, usize Beg, usize End>
using BinarySearchBase = std::conditional_t<
    (End - Beg <= 1), std::integral_constant<usize, Beg>,
    std::conditional_t<Predicate<(Beg + End) / 2>::value,
                       BinarySearch<Predicate, (Beg + End) / 2, End>,
                       BinarySearch<Predicate, Beg, (Beg + End) / 2>>>;

template <template <usize> typename Predicate, usize Beg, usize End>
struct BinarySearch : BinarySearchBase<Predicate, Beg, End> {};

template <template <usize> typename Predicate, usize Beg, usize End>
constexpr usize binary_search_v = BinarySearch<Predicate, Beg, End>::value;

template <template <usize> typename Predicate, usize N>
struct ForwardSearch
    : std::conditional_t<Predicate<N>::value, std::integral_constant<usize, N>,
                         ForwardSearch<Predicate, N + 1>> {};

template <template <usize> typename Predicate, usize N>
struct BackwardSearch
    : std::conditional_t<Predicate<N>::value, std::integral_constant<usize, N>,
                         BackwardSearch<Predicate, N - 1>> {};

template <template <usize> typename Predicate>
struct BackwardSearch<Predicate, -1ull> {};

namespace impl {
template <typename T>
requires std::is_aggregate_v<T>
struct aggregate_inquiry {
  template <usize N>
  struct initializable : std::bool_constant<AggregateInitWithNArgs<T, N>> {};
};

template <Aggregate T>
struct minimum_initialization
    : ForwardSearch<impl::aggregate_inquiry<T>::template initializable, 0> {};

template <Aggregate T>
constexpr auto minimum_initialization_v = minimum_initialization<T>::value;
} // namespace impl

static constexpr auto NUM_BITS = 8;

template <Aggregate T>
struct num_aggregate_fields
    : BinarySearch<impl::aggregate_inquiry<T>::template initializable,
                   impl::minimum_initialization_v<T>,
                   NUM_BITS * sizeof(T) + 1> {};

template <Aggregate T>
constexpr usize num_aggregate_fields_v = num_aggregate_fields<T>::value;

namespace impl {
template <Aggregate T, typename Indices, typename FieldIndices>
struct aggregate_with_indices_initializable_with;

template <Aggregate T, usize... Indices, usize... FieldIndices>
    struct aggregate_with_indices_initializable_with<
        T, std::index_sequence<Indices...>,
        std::index_sequence<FieldIndices...>> : std::bool_constant < requires {
  T{std::declval<IndexedUniversal<Indices>>()...,
    {std::declval<IndexedUniversal<FieldIndices>>()...}};
} > {};
} // namespace impl

template <typename T, usize N, usize M>
concept aggregate_field_n_initializable_with_m_args =
    Aggregate<T> && impl::aggregate_with_indices_initializable_with<
        T, std::make_index_sequence<N>, std::make_index_sequence<M>>::value;

namespace impl {
template <Aggregate T, usize N> struct aggregate_field_inquiry1 {
  template <usize M>
  struct initializable
      : std::bool_constant<
            aggregate_field_n_initializable_with_m_args<T, N, M>> {};
};
} // namespace impl

template <Aggregate T, usize N>
struct num_fields_on_aggregate_field1
    : BinarySearch<impl::aggregate_field_inquiry1<T, N>::template initializable,
                   0, NUM_BITS * sizeof(T) + 1> {};

template <Aggregate T, usize N>
constexpr usize num_fields_on_aggregate_field1_v =
    num_fields_on_aggregate_field1<T, N>::value;

namespace impl {
template <Aggregate T, typename Indices, typename AfterIndices>
struct aggregate_with_indices_initializable_after;

template <Aggregate T, usize... Indices, usize... AfterIndices>
    struct aggregate_with_indices_initializable_after<
        T, std::index_sequence<Indices...>,
        std::index_sequence<AfterIndices...>> : std::bool_constant < requires {
  T{std::declval<IndexedUniversal<Indices>>()...,
    {std::declval<Universal>()},
    std::declval<IndexedUniversal<AfterIndices>>()...};
} > {};
} // namespace impl

template <typename T, usize N, usize K>
concept aggregate_initializable_after_n_with_k_args =
    Aggregate<T> && impl::aggregate_with_indices_initializable_after<
        T, std::make_index_sequence<N>, std::make_index_sequence<K>>::value;

namespace impl {
template <Aggregate T, usize N> struct aggregate_field_inquiry2 {
  template <usize K>
  struct initializable
      : std::bool_constant<
            aggregate_initializable_after_n_with_k_args<T, N, K>> {};

  template <usize K>
  struct not_initializable_m1 : std::negation<initializable<K - 1>> {};
};

template <Aggregate T, usize N>
struct le_max_init_after_n
    : BackwardSearch<aggregate_field_inquiry2<T, N>::template initializable,
                     minimum_initialization_v<T>> {};

template <Aggregate T, usize N>
constexpr usize le_max_init_after_n_v = le_max_init_after_n<T, N>::value;

template <Aggregate T, usize N>
struct min_init_after_n
    : BinarySearch<
          aggregate_field_inquiry2<T, N>::template not_initializable_m1, 1,
          1 + le_max_init_after_n_v<T, N>> {};

template <Aggregate T, usize N>
constexpr usize min_init_after_n_v = min_init_after_n<T, N>::value;

template <typename T, usize N>
concept aggregate_field_n_init = Aggregate<T> && requires {
  le_max_init_after_n<T, N>::value;
};

template <Aggregate T, usize N, bool Initializable>
struct num_fields : std::integral_constant<usize, 1> {};

template <Aggregate T, usize N>
struct num_fields<T, N, true>
    : std::integral_constant<usize,
                             std::max(impl::minimum_initialization_v<T> - N -
                                          impl::min_init_after_n_v<T, N>,
                                      1ull)> {};
} // namespace impl

template <Aggregate T, usize N>
struct num_fields_on_aggregate_field2
    : impl::num_fields<T, N, impl::aggregate_field_n_init<T, N>> {};

template <Aggregate T, usize N>
constexpr usize num_fields_on_aggregate_field2_v =
    num_fields_on_aggregate_field2<T, N>::value;

template <Aggregate T, usize N>
struct num_fields_on_aggregate_field
    : std::conditional_t<(N >= impl::minimum_initialization_v<T>),
                         num_fields_on_aggregate_field1<T, N>,
                         num_fields_on_aggregate_field2<T, N>> {};

template <Aggregate T, usize N>
constexpr usize num_fields_on_aggregate_field_v =
    num_fields_on_aggregate_field<T, N>::value;

namespace impl {
template <usize Val, usize TotalFields>
constexpr auto detect_special_type = Val > TotalFields ? 1 : Val;

template <Aggregate T, usize CurField, usize TotalFields,
          usize CountUniqueFields>
struct unique_field_counter
    : unique_field_counter<
          T,
          CurField +
              detect_special_type<num_fields_on_aggregate_field_v<T, CurField>,
                                  TotalFields>,
          TotalFields, CountUniqueFields + 1> {};

template <Aggregate T, usize Fields, usize UniqueFields>
struct unique_field_counter<T, Fields, Fields, UniqueFields>
    : std::integral_constant<usize, UniqueFields> {};
} // namespace impl

template <Aggregate T>
struct num_aggregate_unique_fields
    : impl::unique_field_counter<T, 0, num_aggregate_fields_v<T>, 0> {};

template <Aggregate T>
constexpr usize unique_fields_v = num_aggregate_unique_fields<T>::value;

static constexpr auto MAX_VISIT = 20;

// NOLINTBEGIN
constexpr auto visit_members(auto &&object, auto &&visitor) -> decltype(auto) {
  const auto field_count =
      unique_fields_v<std::remove_cvref_t<decltype(object)>>;

  if constexpr (field_count > MAX_VISIT) {
    panic("Too many fields to visit.");
  }

  else if constexpr (field_count == 0) {
    return visitor();
  }

  else if constexpr (field_count == 1) {
    auto &&[a1] = object;
    return visitor(a1);
  }

  else if constexpr (field_count == 2) {
    auto &&[a1, a2] = object;
    return visitor(a1, a2);
  }

  else if constexpr (field_count == 3) {
    auto &&[a1, a2, a3] = object;
    return visitor(a1, a2, a3);
  }

  else if constexpr (field_count == 4) {
    auto &&[a1, a2, a3, a4] = object;
    return visitor(a1, a2, a3, a4);
  }

  else if constexpr (field_count == 5) {
    auto &&[a1, a2, a3, a4, a5] = object;
    return visitor(a1, a2, a3, a4, a5);
  }

  else if constexpr (field_count == 6) {
    auto &&[a1, a2, a3, a4, a5, a6] = object;
    return visitor(a1, a2, a3, a4, a5, a6);
  }

  else if constexpr (field_count == 7) {
    auto &&[a1, a2, a3, a4, a5, a6, a7] = object;
    return visitor(a1, a2, a3, a4, a5, a6, a7);
  }

  else if constexpr (field_count == 8) {
    auto &&[a1, a2, a3, a4, a5, a6, a7, a8] = object;
    return visitor(a1, a2, a3, a4, a5, a6, a7, a8);
  }

  else if constexpr (field_count == 9) {
    auto &&[a1, a2, a3, a4, a5, a6, a7, a8, a9] = object;
    return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9);
  }

  else if constexpr (field_count == 10) {
    auto &&[a1, a2, a3, a4, a5, a6, a7, a8, a9, b1] = object;
    return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, b1);
  }

  else if constexpr (field_count == 11) {
    auto &&[a1, a2, a3, a4, a5, a6, a7, a8, a9, b1, b2] = object;
    return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, b1, b2);
  }

  else if constexpr (field_count == 12) {
    auto &&[a1, a2, a3, a4, a5, a6, a7, a8, a9, b1, b2, b3] = object;
    return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, b1, b2, b3);
  }

  else if constexpr (field_count == 13) {
    auto &&[a1, a2, a3, a4, a5, a6, a7, a8, a9, b1, b2, b3, b4] = object;
    return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, b1, b2, b3, b4);
  }

  else if constexpr (field_count == 14) {
    auto &&[a1, a2, a3, a4, a5, a6, a7, a8, a9, b1, b2, b3, b4, b5] = object;
    return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, b1, b2, b3, b4, b5);
  }

  else if constexpr (field_count == 15) {
    auto &&[a1, a2, a3, a4, a5, a6, a7, a8, a9, b1, b2, b3, b4, b5, b6] =
        object;
    return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, b1, b2, b3, b4, b5, b6);
  }

  else if constexpr (field_count == 16) {
    auto &&[a1, a2, a3, a4, a5, a6, a7, a8, a9, b1, b2, b3, b4, b5, b6, b7] =
        object;
    return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, b1, b2, b3, b4, b5, b6,
                   b7);
  }

  else if constexpr (field_count == 17) {
    auto &&[a1, a2, a3, a4, a5, a6, a7, a8, a9, b1, b2, b3, b4, b5, b6, b7,
            b8] = object;
    return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, b1, b2, b3, b4, b5, b6,
                   b7, b8);
  }

  else if constexpr (field_count == 18) {
    auto &&[a1, a2, a3, a4, a5, a6, a7, a8, a9, b1, b2, b3, b4, b5, b6, b7, b8,
            b9] = object;
    return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, b1, b2, b3, b4, b5, b6,
                   b7, b8, b9);
  }

  else if constexpr (field_count == 19) {
    auto &&[a1, a2, a3, a4, a5, a6, a7, a8, a9, b1, b2, b3, b4, b5, b6, b7, b8,
            b9, c1] = object;
    return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, b1, b2, b3, b4, b5, b6,
                   b7, b8, b9, c1);
  }

  else if constexpr (field_count == 20) {
    auto &&[a1, a2, a3, a4, a5, a6, a7, a8, a9, b1, b2, b3, b4, b5, b6, b7, b8,
            b9, c1, c2] = object;
    return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, b1, b2, b3, b4, b5, b6,
                   b7, b8, b9, c1, c2);
  }

  else {
    panic("Too many fields to visit.");
  }
}
// NOLINTEND

} // namespace exl::reflection
