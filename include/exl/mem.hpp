#pragma once

#include <exl/fmt.hpp>
#include <exl/iter.hpp>
#include <exl/option.hpp>

#include <algorithm>
#include <memory_resource>

namespace exl::traits {
template <typename T>
concept Sized = sizeof(T) > 0;

template <typename T>
concept ToPointer = requires(T ptr) {
                      { ptr.as_ptr() };
                    };

} // namespace exl::traits

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

template <traits::Sized T> struct NonNull {
  using Self = NonNull<T>;
  using Val = T;
  using Ptr = T const *;
  using Ref = T &;

  Ptr ptr{};

  [[nodiscard]] static constexpr auto dangling() noexcept -> Self {
    return Self{ptr::addr(alignof(Val))};
  }

  [[nodiscard]] static constexpr auto from_unchecked(Ptr _ptr) noexcept
      -> Self {
    return Self{.ptr = _ptr};
  }

  [[nodiscard]] static constexpr auto from(Ptr _ptr) noexcept -> Option<Self> {
    if (_ptr == nullptr) {
      return {};
    }
    return Self::from_unchecked(_ptr);
  }
};

template <traits::Sized T> struct RcBox {
  using Self = RcBox<T>;
  using Val = T;

  usize strong{};
  usize weak{};
  T val{};
};

template <traits::Sized T> struct Rc {
  using Self = Rc<T>;
  using Val = T;
  using Ptr = T const *;
  using Ref = T &;
};

template <traits::Sized T, typename A = std::pmr::polymorphic_allocator<T>> struct Box {
  using Self = Box<T, A>;
  using Val = T;
  using Ptr = T *;
  using Ref = T &;
  using Alloc = A;

  Ptr ptr{};

  [[nodiscard]] static constexpr auto from_unchecked(T &&data) noexcept
      -> Self {
  }

  [[nodiscard]] static constexpr auto from(T &&data) noexcept -> Option<Self> {
    auto p = new (std::nothrow) T{std::move(data)}; // NOLINT
    if (p == nullptr) {
      return {};
    }
    return {Self{.ptr = p}};
  }

  Box(const Box &box) = delete;
  auto operator=(const Box &box) -> Box & = delete;

  Box(Box &&box) = delete;
  auto operator=(Self &&box) -> Box & = delete;
};

template <traits::Sized T> struct Slice {
  using Self = Slice<T>;
  using Val = T;
  using Ptr = T *;
  using Ref = T &;

  using It = Iter<T>;
  using CIt = CIter<T>;

  Ptr ptr{};
  usize cap{};

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
