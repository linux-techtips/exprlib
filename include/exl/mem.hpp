#pragma once

#include <exl/fmt.hpp>
#include <exl/iter.hpp>
#include <exl/option.hpp>
#include <exl/traits.hpp>

#include <algorithm>

namespace exl::ptr {

template <typename U, typename T>
[[nodiscard]] constexpr inline auto cast(const T *ptr) -> U * {
  return (U *)(ptr); // NOLINT
}

template <typename T> auto copy(const T *src, T *dst, const usize len) -> T * {
  std::copy(src, src + len, dst);
  return dst;
}

template <typename T>
auto copy_bytes(const T *src, T *dst, const usize len) -> T * {
  std::memcpy(dst, src, len);
  return dst;
}

template <typename T>
[[nodiscard]] constexpr inline auto add(T *ptr, const usize offset) -> T * {
  return ptr + offset;
}

template <typename T>
[[nodiscard]] constexpr inline auto add_bytes(T *ptr, const usize offset)
    -> T * {
  ptr::cast<T>(ptr::cast<u8>(ptr) + offset);
}

template <typename T>
[[nodiscard]] constexpr inline auto sub(T *ptr, const usize offset) -> T * {
  return ptr - offset;
}

template <typename T>
[[nodiscard]] constexpr inline auto sub_bytes(T *ptr, const usize offset)
    -> T * {
  ptr::cast<T>(ptr::cast<u8>(ptr) - offset);
}

template <typename T, typename U>
[[nodiscard]] constexpr inline auto diff(T *rhs, U *lhs) -> ssize {
  return rhs - lhs;
}

template <typename T>
[[nodiscard]] constexpr inline auto offset(const usize offset) -> T * {
  return (T *)(offset); // NOLINT
}

template <typename T> [[nodiscard]] constexpr inline auto as_ref(T *ptr) -> T {
  return *ptr;
}

template <typename T>
[[nodiscard]] constexpr inline auto addr(const T &ptr) -> T * {
  return ptr::cast<T>(&ptr);
}

template <typename T> [[nodiscard]] auto addr(const std::string &str) -> T * {
  return (T *)(std::stoll(str, nullptr, 16)); // NOLINT
}

} // namespace exl::ptr

namespace exl {
template <typename T> struct Slice {
  using Self = Slice<T>;
  using Val = T;
  using Ptr = T *;
  using Ref = T &;

  using It = Iter<T>;
  using CIt = CIter<T>;

  Ptr ptr;
  usize cap;

  [[nodiscard]] constexpr auto as_ptr(const usize offset = 0) const -> Ptr {
    return ptr::add(ptr, offset);
  }

  [[nodiscard]] constexpr auto as_ref(const usize offset = 0) const -> Ref {
    return *this->as_ptr(offset);
  }

  [[nodiscard]] constexpr auto size() const -> usize { return cap - 1; }

  [[nodiscard]] constexpr auto begin() const -> It {
    return It::from_unchecked(ptr);
  }
  [[nodiscard]] constexpr auto end() const -> It {
    return It::from_unchecked(this->as_ptr(cap));
  }

  [[nodiscard]] constexpr auto cbegin() const -> CIt {
    return CIt::from_unchecked(ptr);
  }

  [[nodiscard]] constexpr auto cend() const -> CIt {
    return CIt::from_unchecked(this->as_ptr(cap));
  }

  [[nodiscard]] constexpr auto operator[](const usize offset) -> Ref {
    if (offset >= cap) {
      panic(fmt::format("Tried to index slice of size {}, at offset {}", cap,
                        offset));
    }
    return this->as_ref(offset);
  }

  [[nodiscard]] constexpr auto slice(const usize start, const usize end) const
      -> Option<Self> {
    if ((end > cap) || (start >= end)) {
      return {};
    }

    return {Self(ptr::add(ptr, start), end - start)};
  }

  [[nodiscard]] constexpr explicit Slice(const Ptr _ptr, const usize _cap)
      : ptr{_ptr}, cap{_cap} {}

  [[nodiscard]] static constexpr auto from_unchecked(const Ptr _ptr,
                                                     const usize _cap) -> Self {
    return Self(_ptr, _cap);
  }

  [[nodiscard]] static constexpr auto from_unchecked(traits::Range auto &range)
      -> Self {
    return Self(ptr::addr(*range.begin()), range.size());
  }

  [[nodiscard]] static constexpr auto from(const Ptr _ptr, const usize _cap)
      -> Option<Self> {
    if (_ptr == nullptr || _cap == 0) {
      return {};
    }
    return {Self(_ptr, _cap)};
  }
};

} // namespace exl