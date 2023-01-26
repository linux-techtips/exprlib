#pragma once

#include "fmt/core.h"
#include <type_traits>

namespace exl::traits {

template <typename E>
concept Error = requires(E err) {
                  { err.description() } -> std::same_as<std::string>;
                };

} // namespace exl::traits

template <exl::traits::Error E> struct fmt::formatter<E> {
  constexpr auto parse(fmt::format_parse_context &ctx)
      -> decltype(ctx.begin()) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const E &err, FormatContext &ctx) const -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "{}", err.description());
  }
};
