#pragma once

#include <exl/option.hpp>

namespace exl {

template <typename T> struct Test {
  using Self = Test<T>;
  using Val = T;
  using Ptr = T *;
  using Ref = T &;

  T data;

  [[nodiscard]] constexpr explicit Test(const T &_data) : data{_data} {}

  [[nodiscard]] static constexpr auto from_unchecked(const T &_data) -> Self {
    return Test(_data);
  }

  [[nodiscard]] static constexpr auto from(const T &_data) -> Option<Self> {
    if (_data == 0) {
      return {};
    }
    return {Test(_data)};
  }
};

} // namespace exl