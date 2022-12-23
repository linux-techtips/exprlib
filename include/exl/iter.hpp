#pragma once

#include <exl/traits.hpp>

namespace exl {

template <typename T> struct Iter {
  using Self = Iter<T>;
  using Val = T;
  using Ptr = T *;
  using Ref = T &;

  using value_type = Val;
  using difference_type = ssize;
  using pointer = Ptr;
  using reference = Ref;

  Ptr ptr;

  [[nodiscard]] constexpr auto as_ptr(const usize offset = 0) const -> Ptr {
    return ptr + offset;
  }
  [[nodiscard]] constexpr auto as_ref(const usize offset = 0) const -> Ref {
    return *this->as_ptr(offset);
  }

  constexpr auto next() -> void { ++ptr; }
  constexpr auto prev() -> void { --ptr; }

  [[nodiscard]] friend constexpr auto operator<=>(const Iter &rhs, // NOLINT
                                                  const Iter &lhs) = default;
  [[nodiscard]] constexpr auto operator*() const -> Ref {
    return this->as_ref();
  }
  [[nodiscard]] constexpr auto operator->() const -> Ptr {
    return this->as_ptr();
  }
  constexpr auto operator++() -> void { this->next(); }
  constexpr auto operator--() -> void { this->pref(); }

  [[nodiscard]] constexpr explicit Iter(const Ptr _ptr) : ptr{_ptr} {}

  [[nodiscard]] static constexpr inline auto from_unchecked(const Ptr _ptr)
      -> Self {
    return Iter(_ptr);
  }

  [[nodiscard]] static constexpr inline auto from(const Ptr _ptr)
      -> Option<Self> {
    if (_ptr == nullptr) {
      return {};
    }
    return {Iter(_ptr)};
  }
};

template <typename T> struct CIter {
  using Self = CIter<T>;
  using Val = T;
  using Ptr = T *;
  using Ref = T &;

  using value_type = Val;
  using difference_type = ssize;
  using pointer = Ptr;
  using reference = Ref;

  Ptr ptr;

  [[nodiscard]] constexpr auto as_ptr(const usize offset = 0) const -> Ptr {
    return ptr + offset;
  }
  [[nodiscard]] constexpr auto as_ref(const usize offset = 0) const -> Ref {
    return *this->as_ptr(offset);
  }

  constexpr auto next() -> void { ++ptr; }
  constexpr auto prev() -> void { --ptr; }

  [[nodiscard]] friend constexpr auto operator<=>(const CIter &rhs, // NOLINT
                                                  const CIter &lhs) = default;
  [[nodiscard]] constexpr auto operator*() -> Ref { return this->as_ref(); }
  [[nodiscard]] constexpr auto operator->() -> Ptr { return this->as_ptr(); }
  constexpr auto operator++() -> void { this->next(); }
  constexpr auto operator--() -> void { this->pref(); }

  [[nodiscard]] constexpr explicit CIter(const Ptr _ptr) : ptr{_ptr} {}

  [[nodiscard]] static constexpr auto from_unchecked(const Ptr _ptr) -> Self {
    return CIter(_ptr);
  }

  [[nodiscard]] static constexpr auto from(const Ptr _ptr) -> Option<Self> {
    if (_ptr == nullptr) {
      return {};
    }
    return {CIter(_ptr)};
  }
};

struct RangeIter {
  using Self = RangeIter;

  ssize count{};
  ssize inc{1};

  constexpr auto next() -> void { count += inc; }

  [[nodiscard]] friend constexpr auto
  operator<=>(const RangeIter &rhs, // NOLINT
              const RangeIter &lhs) = default;
  [[nodiscard]] constexpr auto operator*() const -> ssize { return count; }

  constexpr auto operator++() -> void { this->next(); }

  [[nodiscard]] constexpr explicit RangeIter(const ssize _count,
                                             const ssize _inc = 1)
      : count{_count}, inc{_inc} {}

  template <typename... Args>
  [[nodiscard]] static constexpr auto from_unchecked(Args &&...args) -> Self {
    return Self(args...);
  }

  template <typename... Args>
  [[nodiscard]] static constexpr auto from(Args &&...args) -> Option<Self> {
    return {Self(args...)};
  }
};

struct Range {
  using Self = Range;

  using It = RangeIter;

  ssize first{};
  ssize last{};
  ssize inc{1};

  [[nodiscard]] constexpr auto begin() const -> RangeIter {
    return RangeIter::from_unchecked(first, inc);
  }

  [[nodiscard]] constexpr auto end() const -> RangeIter {
    auto max = (inc > 0) ? last + 1 : last - 1;
    return RangeIter::from_unchecked(max, inc);
  }

  [[nodiscard]] constexpr explicit Range(const ssize _first, const ssize _last,
                                         const ssize _inc = 1)
      : first{_first}, last{_last}, inc{_inc} {}

  [[nodiscard]] constexpr explicit Range(const ssize _last) : last{_last} {}

  template <typename... Args>
  [[nodiscard]] static constexpr auto from_unchecked(Args &&...args) -> Self {
    return Self(args...);
  }

  [[nodiscard]] static constexpr auto
  from(const ssize _first, const ssize _last, const ssize _inc = 1)
      -> Option<Self> {
    return {Self(_first, _last, _inc)};
  }
};

} // namespace exl