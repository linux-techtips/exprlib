#pragma once

#include <exl/types.hpp>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <string>

namespace exl {

// NOLINTBEGIN
template <typename Char, usize N> struct BasicFixedString {
  Char str[N + 1] = {0};
  [[nodiscard]] constexpr BasicFixedString(const Char *_str) {
    std::copy_n(_str, std::char_traits<char>::length(_str), str);
  }
};

template <usize N> using FixedString = BasicFixedString<char, N>;

// NOLINTEND

template <typename T, typename... Args>
inline auto print(const T format, const Args &...args) -> void {
  fmt::print(format, args...);
}

template <typename T, typename... Args>
inline auto eprint(const T format, const Args &...args) -> void {
  fmt::print(stderr, format, args...);
}

template <typename T, typename... Args>
inline auto println(const T format, const Args &...args) -> void {
  fmt::print(format, args...);
  std::putc('\n', stdout);
}

template <typename T, typename... Args>
inline auto eprintln(const T format, const Args &...args) -> void {
  fmt::print(stderr, format, args...);
  std::putc('\n', stderr);
}

inline auto center(std::string &str, const usize pad, const char symbol = ' ')
    -> std::string {
  auto res = std::string(pad, symbol);
  auto offset = (pad / 2) - (str.length() / 2);
  for (auto i = 0; i < str.length(); ++i) {
    res[i + offset] = str[i];
  }
  return res;
}

} // namespace exl

template <typename T> struct fmt::formatter<std::reference_wrapper<T>> {
  constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const std::reference_wrapper<T> wrapper, FormatContext &ctx) const
      -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "{}", wrapper.get());
  }
};
